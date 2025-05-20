#include "get_task_schedule_cfg_list.h"

using namespace drogon;
using namespace workflow::frmwork;
using namespace workflow::db;
using namespace workflow::flowsvr;

Status GetTaskScheduleCfgListHandler::HandleInput(std::shared_ptr<api::GetTaskScheduleCfgListReq>& reqBody) {
    return Status::OK;
}

Status GetTaskScheduleCfgListHandler::HandleProcess(std::shared_ptr<api::GetTaskScheduleCfgListReq>& reqBody, api::GetTaskScheduleCfgListRsp& rspBody) {
    // 1. 初始化 DAO 与 drogon-model 列表对象
    // 2. 查询调度配置表，获取所有任务类型的调度参数列表
    // 3. 遍历每条配置记录，填充至 Protobuf 返回结构中
    ScheduleCfgDao cfgDao;
    std::vector<TScheduleCfg> cfgList;
    Status status = cfgDao.GetList(cfgList);
    if (!status.ok()) {
        return status;
    }
    for (auto cfg : cfgList) {
        api::TaskScheduleCfg* apiCfg = rspBody.add_task_schedule_cfg();
        FillDBScheduleCfgModel(cfg, *apiCfg);
    }

    return Status::OK;
}