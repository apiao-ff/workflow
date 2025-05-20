#pragma once
#include "frmwork/api.h"
#include "proto/api.pb.h"
#include "flowsvr/src/dao/task.h"
#include "ctrl.h"

namespace workflow {
namespace flowsvr {

class CreateTaskHandler : public TaskHandlerContext {
public:
    CreateTaskHandler() {}
    ~CreateTaskHandler() = default;
    virtual workflow::frmwork::Status HandleInput(std::shared_ptr<api::CreateTaskReq>& reqBody);
    virtual workflow::frmwork::Status HandleProcess(std::shared_ptr<api::CreateTaskReq>& reqBody, api::CreateTaskRsp& rspBody);
    virtual bool UseCache() { return false; }
    virtual void SetCache() {}
};

using CreateTaskCtrl = TaskCtrlBase<CreateTaskHandler, api::CreateTaskReq, api::CreateTaskRsp>;

}; 
}; 