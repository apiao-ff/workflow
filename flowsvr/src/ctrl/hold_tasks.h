#pragma once


#include "ctrl.h"

namespace workflow {
namespace flowsvr {

class HoldTasksHandler : public TaskHandlerContext {
public:
    HoldTasksHandler() {}
    ~HoldTasksHandler() = default;
    virtual workflow::frmwork::Status HandleInput(std::shared_ptr<api::HoldTasksReq>& reqBody);
    virtual workflow::frmwork::Status HandleProcess(std::shared_ptr<api::HoldTasksReq>& reqBody, api::HoldTasksRsp& rspBody);
    virtual bool UseCache() { return false; }
    virtual void SetCache() {}

    static const int32_t MAX_TASK_LIST_LIMIT;
    static const int32_t DEFAULT_TASK_LIST_LIMIT;
};

using HoldTasksCtrl = TaskCtrlBase<HoldTasksHandler, api::HoldTasksReq, api::HoldTasksRsp>;

}; 
}; 