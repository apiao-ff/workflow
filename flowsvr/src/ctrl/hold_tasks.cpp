#include "hold_tasks.h"
#include "flowsvr/src/dao/task.h"
#include "flowsvr/src/dao/schedule_pos.h"
#include "flowsvr/src/dao/schedule_cfg.h"

using namespace drogon;
using namespace workflow::frmwork;
using namespace workflow::db;
using namespace workflow::flowsvr;

const int32_t HoldTasksHandler::MAX_TASK_LIST_LIMIT = 1000;
const int32_t HoldTasksHandler::DEFAULT_TASK_LIST_LIMIT = 1000;

Status HoldTasksHandler::HandleInput(std::shared_ptr<api::HoldTasksReq>& reqBody) {
    if (reqBody->task_type() == "") {
        LOG_ERROR << "task type is empty";
        return InputInvalid;
    }
    return Status::OK;
}

Status HoldTasksHandler::HandleProcess(std::shared_ptr<api::HoldTasksReq>& reqBody, api::HoldTasksRsp& rspBody) {
    // 1. 初始化 DAO、调度位置结构与任务列表容器
    // 2. 解析任务类型与 limit，并做边界保护（避免过大/为零）
    // 3. 查询调度起始位置（用于选定目标任务表）
    // 4. 获取当前调度分片下状态为 Pending 的任务列表
    // 5. 遍历结果，筛除冷却中的任务，填充响应结构，并收集任务ID
    // 6. 批量将这些任务状态更新为 Processing（加锁操作）

    TaskDao taskDao;
    SchedulePosDao posDao;
    TSchedulePos schPos;
    std::vector<TTask> vecTasks;
    std::vector<std::string> taskIDs;
    int limit = reqBody->limit();
    if (limit > MAX_TASK_LIST_LIMIT) {
        limit = MAX_TASK_LIST_LIMIT;
    }
    if (limit == 0) {
        limit = DEFAULT_TASK_LIST_LIMIT;
    }
    std::string taskType = reqBody->task_type();
    std::string taskTableName = GetTaskTableName(taskType);

    Status status = posDao.Get(taskTableName, schPos);
    if (!status.ok()) {
        return Status::FAIL;
    }

    std::string beginSchPos = std::to_string(schPos.getValueOfScheduleBeginPos());
    status = taskDao.GetTaskList(taskType, beginSchPos, Pending, limit, vecTasks);
    if (!status.ok()) {
        return Status::FAIL;
    }

    for (auto& task : vecTasks) {
        if (task.getValueOfCrtRetryNum() != 0 && task.getValueOfMaxRetryInterval() != 0 &&
            task.getValueOfOrderTime() > TimestampNow()) {
            continue;
        }
        taskIDs.push_back(task.getValueOfTaskId());
        api::TaskData* taskData = rspBody.add_task_list();
        FillPBTaskModel(task, *taskData);
    }

    status = taskDao.BatchSetStatus(taskIDs, Processing);
    if (!status.ok()) {
        LOG_INFO << "BatchSetStatus: " << status.error_code();
        return status;
    }
    return Status::OK;
}