#pragma once

#include "ctrl.h"

namespace workflow {
namespace flowsvr {

class GetTasksListHandler : public TaskHandlerContext {
public:
    GetTasksListHandler() {}
    ~GetTasksListHandler() = default;
    virtual workflow::frmwork::Status HandleInput(std::shared_ptr<api::GetTaskListReq>& reqBody);
    virtual workflow::frmwork::Status HandleProcess(std::shared_ptr<api::GetTaskListReq>& reqBody, api::GetTaskListRsp& rspBody);
    virtual bool UseCache() { return false; }
    virtual void SetCache() {}

    static const int32_t MAX_TASK_LIST_LIMIT;
    static const int32_t DEFAULT_TASK_LIST_LIMIT;
};

using GetTasksListCtrl =
TaskCtrlBase<GetTasksListHandler, api::GetTaskListReq, api::GetTaskListRsp>;

};
}; 