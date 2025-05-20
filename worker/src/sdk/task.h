#pragma once

#include "frmwork/api.h"
#include "proto/api.pb.h"


namespace workflow {
namespace worker {



using Status = workflow::frmwork::Status;
enum TaskStatus { Pending, Processing, Succ, Failed };

class ScheduleData {
public:
    ScheduleData() {}
    ~ScheduleData() = default;
    std::string TraceID;
    std::string ErrMsg;
    std::string Cost;
};
using ScheduleDataPtr = std::shared_ptr<ScheduleData>;

class ScheduleLog {
public:
    ScheduleLog() {}
private:
    std::list<ScheduleDataPtr> logs_;
};

class TaskBase {
public:
    TaskBase(const api::TaskData taskData, const std::string host);
    virtual ~TaskBase();
public:
    virtual Status ContextLoad() = 0;
    // 任务可能有多个阶段  
    // 返回OK 表示本阶段无误
    // 返回FAIL 表示本阶段故障 进入重试流程
    // 若任务还有下一阶段 任务状态应设置为Pending
    // 若任务全阶段完成 任务状态应设置为Succ
    virtual Status HandleProcess() = 0;
    virtual Status handleFailedMust() = 0;
    virtual Status HandleFinishError() = 0;
    virtual void HandleFinish() = 0;
public:
    virtual Status CreateTask(std::string& taskId);
    virtual Status SetTask();
public:
    virtual void SetTaskData(const api::TaskData& taskData);
    api::TaskData& TaskData();

protected:
    api::TaskData taskData_;
    std::string svr_host_;
};
#define CLONE_TASK_PB(word) req.set_##word(taskData_.word())
using TaskPtr = std::shared_ptr<TaskBase>;
using TaskBaseFactory = std::function<TaskPtr(const api::TaskData&, const std::string&)>;
}; 
}; 