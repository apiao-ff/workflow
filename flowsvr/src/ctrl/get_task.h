#pragma once


#include "ctrl.h"

namespace workflow {
namespace flowsvr {

class GetTaskHandler : public TaskHandlerContext {
public:
    GetTaskHandler() {}
    ~GetTaskHandler() = default;
    virtual workflow::frmwork::Status HandleInput(std::shared_ptr<api::GetTaskReq>& reqBody);
    virtual workflow::frmwork::Status HandleProcess(std::shared_ptr<api::GetTaskReq>& reqBody, api::GetTaskRsp& rspBody);
    virtual bool UseCache() { return false; }
    virtual void SetCache() {}
};

using GetTaskCtrl = TaskCtrlBase<GetTaskHandler, api::GetTaskReq, api::GetTaskRsp>;

}; 
}; 