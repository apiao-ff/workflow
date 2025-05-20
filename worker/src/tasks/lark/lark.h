#pragma once

#include "worker/src/sdk/task.h"
#include <thread>

namespace workflow {
namespace worker {

using Status = workflow::frmwork::Status;

class Lark : public TaskBase {
public:
    Lark(const api::TaskData& data, const std::string& host);
    virtual Status ContextLoad() override;
    virtual Status HandleProcess() override; 
    virtual Status handleFailedMust() override;
    virtual Status HandleFinishError() override;
    virtual void HandleFinish() override;
    static TaskPtr CreateLark(const api::TaskData& data, const std::string& host);
};

}; 
}; 