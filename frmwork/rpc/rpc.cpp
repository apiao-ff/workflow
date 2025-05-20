#include "rpc.h"

using namespace workflow::frmwork;

Status TaskRpc::GetTaskScheduleCfgList(
    const std::string& host,
    const api::GetTaskScheduleCfgListReq& req,
    api::GetTaskScheduleCfgListRsp* rsp
) {
    return SendRequest<api::GetTaskScheduleCfgListReq, api::GetTaskScheduleCfgListRsp>
        (host, "/get_task_schedule_cfg_list", req, rsp);
}

Status TaskRpc::HoldTasks(
    const std::string& host,
    const api::HoldTasksReq& req,
    api::HoldTasksRsp* rsp
) {
    return SendRequest<api::HoldTasksReq, api::HoldTasksRsp>
        (host, "/hold_tasks", req, rsp);
}

Status TaskRpc::SetTask(
    const std::string& host,
    const api::SetTaskReq& req,
    api::SetTaskRsp* rsp
) {
    return SendRequest<api::SetTaskReq, api::SetTaskRsp>
        (host, "/set_task", req, rsp);
}

Status TaskRpc::CreateTask(
    const std::string& host,
    const api::CreateTaskReq& req,
    api::CreateTaskRsp* rsp
) {
    return SendRequest<api::CreateTaskReq, api::CreateTaskRsp>
        (host, "/create_task", req, rsp);
}


