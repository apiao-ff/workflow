#include "set_task.h"

#include "flowsvr/src/dao/task.h"
#include "flowsvr/src/dao/schedule_pos.h"
#include "flowsvr/src/dao/schedule_cfg.h"

using namespace drogon;
using namespace workflow::frmwork;
using namespace workflow::db;
using namespace workflow::flowsvr;

Status SetTaskHandler::HandleInput(std::shared_ptr<api::SetTaskReq>& reqBody) {
    if (reqBody->task_id() == "") {
        LOG_ERROR << "task id is empty";
        return InputInvalid;
    }
    return Status::OK;
}

Status SetTaskHandler::HandleProcess(std::shared_ptr<api::SetTaskReq>& reqBody, api::SetTaskRsp& rspBody) {
    // 1. 初始化 DAO 和 drogon-model 实体对象
    // 2. 从请求中逐字段提取数据填充到 ORM 模型中（通过 CLONE_TASK_DBMODEL 宏）
    // 3. 调用 DAO 执行任务记录更新操作

    TaskDao taskDao;
    TTask task;

    // proto::task -> model::task
    // task.setTaskId(reqBody->task_id())
    CLONE_TASK_DBMODEL(TaskId, task_id);
    CLONE_TASK_DBMODEL(UserId, user_id);
    CLONE_TASK_DBMODEL(TaskType, task_type);
    CLONE_TASK_DBMODEL(TaskStage, task_stage);
    CLONE_TASK_DBMODEL(Status, status);
    CLONE_TASK_DBMODEL(Priority, priority);
    CLONE_TASK_DBMODEL(CrtRetryNum, crt_retry_num);
    CLONE_TASK_DBMODEL(MaxRetryNum, max_retry_num);
    CLONE_TASK_DBMODEL(MaxRetryInterval, max_retry_interval);
    CLONE_TASK_DBMODEL(ScheduleLog, schedule_log);
    CLONE_TASK_DBMODEL(TaskContext, task_context);
    CLONE_TASK_DBMODEL(OrderTime, order_time);
    // 修改时间和创建时间不支持修改由数据库设置

    Status status = taskDao.Save(task);
    if (!status.ok()) {
        return status;
    }
    return Status::OK;
}
