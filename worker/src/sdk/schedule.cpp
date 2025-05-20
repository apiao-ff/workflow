#include "schedule.h"

#include <chrono>
#include <thread>
#include <algorithm>
using namespace workflow::worker;
using namespace workflow::frmwork;

const int LOADINGCFG_INTERVAL = 20;

// drogon::task -> proto::task
TaskMgr::TaskMgr(const std::string& taskType, const std::string& svrHost, const std::string& redisHost, TaskBaseFactory factory)
    :taskType_(taskType), taskSvrHost_(svrHost), redisLock_(redisHost), factory_(factory)
{
    stop_ = false;
}

void TaskMgr::loadCfg() {
    api::GetTaskScheduleCfgListReq req;
    std::unique_ptr<api::GetTaskScheduleCfgListRsp> rspPtr = std::make_unique<api::GetTaskScheduleCfgListRsp>();
    Status status = TaskRpc::GetTaskScheduleCfgList(taskSvrHost_, req, rspPtr.get());
    if (!status.ok()) {
        LOG_ERROR << "GetTaskScheduleCfgList err: " << status.error_code();
        return;
    }
    std::map<std::string, api::TaskScheduleCfg> newMap;
    for (int i = 0; i < rspPtr->task_schedule_cfg_size(); i++) {
        newMap[rspPtr->task_schedule_cfg(i).task_type()] = rspPtr->task_schedule_cfg(i);
    }
    {
        std::lock_guard<std::mutex> guard(lock_);
        cfgMap_ = newMap;
    }
}

void TaskMgr::keepLoadCfg() {
    std::thread t([&]() {
        while (!stop_) {
            loadCfg();
            std::this_thread::sleep_for(std::chrono::seconds(LOADINGCFG_INTERVAL));
        }
        });
    t.detach();
}

void TaskMgr::Init() {
    LOG_INFO << "init";
    keepLoadCfg();
}

void TaskMgr::Loop() {
    LOG_INFO << "begin to schedule";
    while (!stop_) {
        api::TaskScheduleCfg scheduleCfg;
        {
            std::lock_guard<std::mutex> guard(lock_);
            // 确定本次调度的配置
            scheduleCfg = cfgMap_[taskType_];
            // 调整线程数量
            int expectThreadNum = std::max((int32_t)std::thread::hardware_concurrency(), scheduleCfg.schedule_limit() / 4);
            while (threadPool_.get_threads_num() < expectThreadNum) {
                threadPool_.add_thread();
            };
            while (threadPool_.get_threads_num() > expectThreadNum) {
                threadPool_.delete_thread();
            };
        }
        LOG_INFO << "schedule one time";
        std::thread scheduleThread(std::bind(&TaskMgr::schedule, this, scheduleCfg));
        scheduleThread.join();
        LOG_INFO << "schedule done";
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    }
}

void TaskMgr::schedule(api::TaskScheduleCfg scheduleCfg) {
    std::vector<TaskPtr> tasks;
    Hold(scheduleCfg, tasks);
    if (tasks.size() == 0) {
        LOG_INFO << "no task";
        return;
    }

    // std::vector<std::thread> ths;
    // for (auto taskPtr : tasks) {
    //     ths.push_back(std::thread(RunTask, scheduleCfg, taskPtr));
    // }
    // for (auto& t : ths) {
    //     t.join();
    // }

    // 调度线程不等待本批次任务执行结束 因此可能出现多个批次任务堆积在线程池
    if (threadPool_.get_tasks_size() > 2 * scheduleCfg.schedule_limit()) {
        LOG_WARN << "scheduling delay";
        threadPool_.wait_tasks(3000);
    }
    for (auto taskPtr : tasks) {
        threadPool_.submit(RunTask, scheduleCfg, taskPtr);
    }
}

void TaskMgr::Hold(const api::TaskScheduleCfg& cfg, std::vector<TaskPtr>& tasks) {
    api::HoldTasksReq req;
    std::unique_ptr<api::HoldTasksRsp> rspPtr = std::make_unique<api::HoldTasksRsp>();

    req.set_task_type(taskType_);
    req.set_limit(cfg.schedule_limit());

    if (!redisLock_.lock("lock:" + taskType_, 30000)) {
        LOG_INFO << "did not acquire lock, skipping...";
        return;
    }
    Status status = TaskRpc::HoldTasks(taskSvrHost_, req, rspPtr.get());
    redisLock_.unlock("lock:" + taskType_);

    if (!status.ok()) {
        LOG_ERROR << "hold tasks fail ";
    }
    if (rspPtr->task_list_size() == 0) {
        LOG_INFO << "hold tasks size is empty";
        return;
    }

    for (int i = 0; i < rspPtr->task_list_size(); i++) {
        TaskPtr taskPtr = factory_(rspPtr->task_list(i), taskSvrHost_);
        tasks.push_back(taskPtr);
    }
}

void TaskMgr::RunTask(const api::TaskScheduleCfg& cfg, TaskPtr taskPtr) {
    Status status;
    uint32_t beginTs;
    uint32_t delayTs;
    api::TaskData& taskData = taskPtr->TaskData();

    // status 表示阶段失败或者成功
    // Succ Failed 表示最终失败或者最终成功


    // 加载上下文
    status = taskPtr->ContextLoad();
    if (!status.ok()) {
        LOG_ERROR << "context load error" << status.error_code();
    }
    else {
        status = taskPtr->HandleProcess();
    }

    // 阶段性失败 进入重试流程
    if (!status.ok()) {
        // 超出最大重试次数 判定为最终失败
        if (taskData.max_retry_num() == 0 || taskData.crt_retry_num() > taskData.max_retry_num()) {
            taskData.set_status(Failed);
        }
        // 等待将来重试
        else {
            // 重新设置等待态 更新重试次数 调整重试间隔
            taskData.set_status(Pending);
            taskData.set_crt_retry_num(taskData.crt_retry_num() + 1);
            delayTs = cfg.max_retry_interval();
            if (delayTs < 0) delayTs *= -1;
            else delayTs *= 2;
            taskData.set_order_time(TimestampNow() + delayTs);
            LOG_ERROR << "handleProcess error" << status.error_code();
        }
    }

    // 任务最终失败额外流程
    if (taskData.status() == Failed) {
        // 尝试强制补偿
        status = taskPtr->handleFailedMust();
        if (!status.ok()) {
            LOG_ERROR << "handleFailedMust error" << status.error_code();
        }
        // 正常失败处理
        status = taskPtr->HandleFinishError();
        if (!status.ok()) {
            LOG_ERROR << "HandleFinishError error" << status.error_code();
        }
    }

    // 任务最终完成 或者 任务最终失败
    if (taskData.status() == Failed || taskData.status() == Succ) {
        taskPtr->HandleFinish();
    }

    // 更新
    taskPtr->SetTask();
}