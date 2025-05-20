#pragma once

#include "frmwork/comm/comm.h"
#include "frmwork/status/status.h"
#include "proto/api.pb.h"

namespace workflow {
namespace frmwork {

template <typename ReqBody, typename RspBody>
Status SendRequest(const std::string& host, const std::string& URL, const ReqBody& reqBody, RspBody* rspBody)
{
    std::string stReqBody;
    Status status;
    ProtobufUtil::MessageToJsonString(reqBody, &stReqBody);
    auto client = drogon::HttpClient::newHttpClient(host);
    auto req = drogon::HttpRequest::newHttpRequest();
    req->setMethod(drogon::Post);
    req->setPath(URL);
    req->setBody(stReqBody);
    req->setContentTypeCode(drogon::CT_APPLICATION_JSON);
    // client->sendRequest(req,
    //     [&rspBody, &status](drogon::ReqResult result, const drogon::HttpResponsePtr& response)
    //     {
    //         if (result != ReqResult::Ok)
    //         {
    //             LOG_ERROR << "error while sending request to server! result: " << result;
    //             status.set(ERR_HTTP_REQUEST_ERR, "error code: " + std::to_string(int(result)));
    //             return;
    //         }
    //         if (!ProtobufUtil::JsonStringToMessage(std::string(response->body()), rspBody).ok())
    //         {
    //             status.set(rspBody->code(), rspBody->msg());
    //         }
    //     }
    // );
    std::pair<drogon::ReqResult, drogon::HttpResponsePtr> rspPair = client->sendRequest(req);
    // LOG_INFO << "Response body: " << rspPair.second->getBody();
    if (!ProtobufUtil::JsonStringToMessage(std::string(rspPair.second->getBody().data()), rspBody).ok()){
        status.set(rspBody->code(), rspBody->msg());
    }
    return status;
};

class TaskRpc {
public:
    static Status GetTaskScheduleCfgList(
        const std::string& host, 
        const api::GetTaskScheduleCfgListReq& req, 
        api::GetTaskScheduleCfgListRsp* rsp
    );

    static Status HoldTasks(
        const std::string& host, 
        const api::HoldTasksReq& req, 
        api::HoldTasksRsp* rsp
    );

    static Status SetTask(
        const std::string& host, 
        const api::SetTaskReq& req, 
        api::SetTaskRsp* rsp
    );
    
    static Status CreateTask(
        const std::string& host, 
        const api::CreateTaskReq& req, 
        api::CreateTaskRsp* rsp
    );
};

}; 
}; 