#pragma once


#include "ctrl.h"

namespace workflow {
namespace flowsvr {

class GetTaskScheduleCfgListHandler : public TaskHandlerContext {
public:
    GetTaskScheduleCfgListHandler() {}
    ~GetTaskScheduleCfgListHandler() = default;
    virtual workflow::frmwork::Status HandleInput(std::shared_ptr<api::GetTaskScheduleCfgListReq>& reqBody);
    virtual workflow::frmwork::Status HandleProcess(std::shared_ptr<api::GetTaskScheduleCfgListReq>& reqBody, api::GetTaskScheduleCfgListRsp& rspBody);
    virtual bool UseCache() { return false; }
    virtual void SetCache() {}
};

using GetTaskScheduleCfgListCtrl = TaskCtrlBase<GetTaskScheduleCfgListHandler, api::GetTaskScheduleCfgListReq, api::GetTaskScheduleCfgListRsp>;
}; 
}; 