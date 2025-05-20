#include "task.h"

using namespace workflow::worker;
using namespace workflow::frmwork;

// 
TaskBase::TaskBase(const api::TaskData taskData, const std::string host) : taskData_(taskData), svr_host_(host) {}
TaskBase::~TaskBase() {};

void TaskBase::SetTaskData(const api::TaskData& taskData) {
    taskData_ = taskData;
}

api::TaskData& TaskBase::TaskData() {
    return taskData_;
}

// proto::task -> drogon::task
Status TaskBase::SetTask() {
    api::SetTaskReq req;
    api::SetTaskRsp rsp;
    CLONE_TASK_PB(user_id);
    CLONE_TASK_PB(task_id);
    CLONE_TASK_PB(task_type);
    CLONE_TASK_PB(task_stage);
    CLONE_TASK_PB(status);
    CLONE_TASK_PB(priority);
    CLONE_TASK_PB(crt_retry_num);
    CLONE_TASK_PB(max_retry_num);
    CLONE_TASK_PB(max_retry_interval);
    CLONE_TASK_PB(schedule_log);
    CLONE_TASK_PB(task_context);
    CLONE_TASK_PB(order_time);
    return TaskRpc::SetTask(svr_host_, req, &rsp);
}

Status TaskBase::CreateTask(std::string& taskID) {
    api::CreateTaskReq req;
    api::CreateTaskRsp rsp;
    *(req.mutable_taskdata()) = taskData_;
    Status status = TaskRpc::CreateTask(svr_host_, req, &rsp);
    taskID = rsp.task_id();
    return status;
}