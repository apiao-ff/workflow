#pragma once

#include "frmwork/api.h"
#include "flowsvr/src/dao/api.h"
#include "proto/api.pb.h"

namespace workflow {
namespace flowsvr {

template <class TaskHandler, class ReqProto, class RspProto>
class TaskCtrlBase : public workflow::frmwork::HandlerBase<ReqProto, RspProto> {

public:
    virtual HttpResponsePtr Handle(const HttpRequestPtr& reqHTTP, std::shared_ptr<ReqProto>& reqBodyPtr) {
        workflow::frmwork::Status status;
        RspProto rspBody;
        std::unique_ptr<TaskHandler> th = std::make_unique<TaskHandler>();
        status = th->HandleInput(reqBodyPtr);
        if (!status.ok()) {
            return this->ReplyFail(status, rspBody);
        }
        status = th->HandleProcess(reqBodyPtr, rspBody);
        if (!status.ok()) {
            return this->ReplyFail(status, rspBody);
        }
        return this->ReplySucc(rspBody);
    }
};

class TaskHandlerContext {
public:
    void SetCtx(const std::string& userID) { user_id_ = userID; }

protected:
    std::string user_id_;
};

};
};

void FillDBTaskModel(const api::TaskData& taskdata, workflow::db::TTask& ttask);

void FillPBTaskModel(const workflow::db::TTask& ttask, api::TaskData& taskdata);

void FillDBScheduleCfgModel(const workflow::db::TScheduleCfg& tcfg, api::TaskScheduleCfg& cfg);