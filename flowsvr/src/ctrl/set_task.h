#pragma once

#include "ctrl.h"

namespace workflow {
namespace flowsvr {

class SetTaskHandler : public TaskHandlerContext {
public:
    SetTaskHandler() {}
    ~SetTaskHandler() = default;
    virtual workflow::frmwork::Status HandleInput(std::shared_ptr<api::SetTaskReq>& reqBody);
    virtual workflow::frmwork::Status HandleProcess(std::shared_ptr<api::SetTaskReq>& reqBody, api::SetTaskRsp& rspBody);
    virtual bool UseCache() { return false; }
    virtual void SetCache() {}
};

#define CLONE_TASK_DBMODEL(dbword, pbword) task.set##dbword(reqBody->pbword())

using SetTaskCtrl = TaskCtrlBase<SetTaskHandler, api::SetTaskReq, api::SetTaskRsp>;

}; 
}; 