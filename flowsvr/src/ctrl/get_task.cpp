#include "get_task.h"

#include "flowsvr/src/dao/task.h"
#include "flowsvr/src/dao/schedule_pos.h"
#include "flowsvr/src/dao/schedule_cfg.h"

using namespace drogon;
using namespace workflow::frmwork;
using namespace workflow::db;
using namespace workflow::flowsvr;

Status GetTaskHandler::HandleInput(std::shared_ptr<api::GetTaskReq>& reqBody) {
    if (reqBody->task_id() == "") {
        return Status::FAIL;
    }
    return Status::OK;
}

Status GetTaskHandler::HandleProcess(std::shared_ptr<api::GetTaskReq>& reqBody, api::GetTaskRsp& rspBody) {
    // 1. 初始化 DAO 和 drogon-model 实体对象
    // 2. 根据 task_id 查询对应分表任务记录
    // 3. 将 ORM 实体转换为 Protobuf 响应结构
    // 4. 返回查询结果
    TaskDao taskDao;
    TTask ttask;
    Status status = taskDao.Find(reqBody->task_id(), ttask);
    if (!status.ok()) {
        return status;
    }
    FillPBTaskModel(ttask, *rspBody.mutable_task_data());
    return Status::OK;
}
