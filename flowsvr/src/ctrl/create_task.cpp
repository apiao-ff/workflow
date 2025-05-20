#include "create_task.h"
#include "flowsvr/src/dao/task.h"
#include "flowsvr/src/dao/schedule_pos.h"
#include "flowsvr/src/dao/schedule_cfg.h"

using namespace drogon;
using namespace workflow::frmwork;
using namespace workflow::db;
using namespace workflow::flowsvr;

Status CreateTaskHandler::HandleInput(std::shared_ptr<api::CreateTaskReq>& reqBody) {
    if (reqBody->taskdata().task_type() == "") {
        LOG_ERROR << "task type is empty";
        return InputInvalid;
    }
    return Status::OK;
}

Status CreateTaskHandler::HandleProcess(std::shared_ptr<api::CreateTaskReq>& reqBody, api::CreateTaskRsp& rspBody) {
    // 1. 查询调度进度表，确定插入任务的目标分表位置
    // 2. 查询调度策略表，补全任务的重试次数与间隔等配置字段
    // 3. 将 Protobuf 请求结构转换为 ORM 模型对象并写入对应任务分表
    // 4. 设置响应字段（如 task_id）并返回
    TaskDao taskDao;
    SchedulePosDao posDao;
    ScheduleCfgDao cfgDao;
    TScheduleCfg scheduleCfg;
    TSchedulePos schPos;
    TTask task;
    Status status;
    std::string taskType = reqBody->taskdata().task_type();
    std::string taskTableName = GetTaskTableName(taskType);
    status = posDao.Get(taskTableName, schPos);
    if (!status.ok()) {
        return Status::FAIL;
    }
    std::string schedulePosStr = std::to_string(schPos.getValueOfScheduleEndPos());
    status = cfgDao.Get(taskType, scheduleCfg);
    if (!status.ok()) {
        return Status::FAIL;
    }
    // 最大重试次数，最大重试间隔，ordertime(考虑优先级)字段 
    reqBody->mutable_taskdata()->set_status(0);
    reqBody->mutable_taskdata()->set_schedule_log("");
    reqBody->mutable_taskdata()->set_crt_retry_num(0);
    reqBody->mutable_taskdata()->set_max_retry_interval(scheduleCfg.getValueOfRetryInterval());
    reqBody->mutable_taskdata()->set_max_retry_num(scheduleCfg.getValueOfMaxRetryNum());
    reqBody->mutable_taskdata()->set_order_time(TimestampNow() - uint32_t(reqBody->taskdata().priority()));
    FillDBTaskModel(reqBody->taskdata(), task);
    status = taskDao.Create(taskType, schedulePosStr, task);
    if (!status.ok()) {
        return status;
    }
    rspBody.set_task_id(task.getValueOfTaskId());
    return Status::OK;
}
