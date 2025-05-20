#pragma once

#include <drogon/drogon.h>

#include "frmwork/comm/comm.h"
#include "frmwork/status/status.h"
#include "frmwork/const/const.h"

using namespace drogon;

namespace workflow {
namespace frmwork {

#define GETUSERID std::string userID = reqHTTP->getHeader("User-Id");

// Handler handler
class Handler
{
public:
    virtual ~Handler() = default;
    virtual int Process(const HttpRequestPtr& reqHTTP, std::function<void(const HttpResponsePtr&)>&& callback) = 0;
};

// RegisterHandler register handler
template <class handler>
void RegisterHandler(HttpAppFramework& app, const std::string& path) {
    app.registerHandler(path,
        [](const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback)
        {
            auto h = std::make_unique<handler>();
            h->Process(req, std::move(callback));
        }
    );
}

// HandlerBase base http handler
template <class ReqProto, class RspProto>
class HandlerBase : public Handler
{
public:
    virtual ~HandlerBase() = default;

    // Handle main entry
    virtual HttpResponsePtr Handle(const HttpRequestPtr& reqHTTP, std::shared_ptr<ReqProto>& reqBody) = 0;

    // ReplySucc reply success
    virtual HttpResponsePtr ReplySucc(RspProto& rsp)
    {
        return Reply(Status::OK.error_code(), Status::OK.error_message(), rsp);
    }

    // ReplyFail reply fail
    virtual HttpResponsePtr ReplyFail(Status status, RspProto& rsp)
    {
        return Reply(status.error_code(), status.error_message(), rsp);
    }

    // Reply reply
    virtual HttpResponsePtr Reply(int code, const std::string& msg, RspProto& rsp)
    {
        rsp.set_code(code);
        rsp.set_msg(msg);
        std::string strReply;
        ProtobufUtil::JsonPrintOptions opt;
        opt.always_print_primitive_fields = true;
        ProtobufUtil::Status status = ProtobufUtil::MessageToJsonString(rsp, &strReply, opt);
        auto resp = HttpResponse::newHttpResponse();
        resp->setBody(strReply);
        resp->setStatusCode(k200OK);
        return resp;
    }

    virtual std::shared_ptr<ReqProto> createEmptyReqProto()
    {
        return std::make_shared<ReqProto>();
    }

// Process, called by framework
    virtual int Process(const HttpRequestPtr& reqHTTP, std::function<void(const HttpResponsePtr&)>&& callback)
    {
        // get json body
        std::shared_ptr<ReqProto> reqPtr = createEmptyReqProto();
        ProtobufUtil::Status status = ProtobufUtil::JsonStringToMessage(reqHTTP->bodyData(), reqPtr.get());
        if (!status.ok())
        {
            RspProto rsp;
            callback(ReplyFail(InputInvalid, rsp));
            return 0;
        }
        auto rspPtr = Handle(reqHTTP, reqPtr);
        callback(rspPtr);
        return 0;
    }
};
};
};