#pragma once

#include "dao.h"

namespace workflow {
namespace db {

using namespace drogon_model::workflow;


enum TaskStatus { Pending, Processing, Succ, Failed };

class TaskDao : public Dao {
public:
    TaskDao();

    // ===== 工具函数 =====
    static bool IsValidStatus(int32_t status);
    void GetTablePosFromTaskID(const std::string& taskID, std::string& taskType, std::string& pos);
    std::string GetTableName(const std::string& taskType, const std::string& pos);
    std::string GenTaskID(const std::string& taskType, const std::string& pos);
    workflow::frmwork::Status CreateNextTable(const std::string& taskType, const std::string& pos);

    // ===== 任务创建 =====
    workflow::frmwork::Status Create(const std::string& taskType, const std::string& pos, TTask& task);

    // ===== 查询接口 =====
    workflow::frmwork::Status Find(const std::string& taskID, TTask& task);
    workflow::frmwork::Status GetTaskList(const std::string& taskType, const std::string& pos, const TaskStatus& status, int limit, std::vector<TTask>& tasklist);
    workflow::frmwork::Status GetAliveTaskList(const std::string& taskType, const std::string& pos, const TaskStatus& status, int limit, std::vector<TTask>& tasklist);
    workflow::frmwork::Status GetAliveTaskCount(const std::string& taskType, const std::string& pos, int& count);
    workflow::frmwork::Status GetAllTaskCount(const std::string& taskType, const std::string& pos, int& count);

    // ===== 修改接口 =====
    workflow::frmwork::Status SetStatus(const std::string& taskID, const TaskStatus& status);
    workflow::frmwork::Status BatchSetStatus(const std::vector<std::string>& taskIDList, const TaskStatus& status);
    workflow::frmwork::Status SetContext(const std::string& taskID, const std::string& context);
    workflow::frmwork::Status Save(TTask& task);

    // ===== 异常处理/特殊流程 =====
    workflow::frmwork::Status GetLongTimeProcessing(const std::string& taskType, const std::string& pos, const int& maxProcessTime, const int& limit, std::vector<TTask>& tasklist);
    workflow::frmwork::Status ModifyTimeoutPending(const std::string& taskType, const std::string& pos, const int& maxProcessTime);
    workflow::frmwork::Status IncreaseCrtRetryNum(const std::string& taskID);
    workflow::frmwork::Status SetScheduleLog(const std::string& taskID, const std::string& log);

    // 其他
    workflow::frmwork::Status GetTaskCountByStatus(const std::string& taskType, const std::string& pos, const TaskStatus& status, int& count);
    workflow::frmwork::Status BatchSetOwnerStatusWithPendingOutModify(const std::vector<std::string>& taskIDs, const std::string& owner, const TaskStatus& status);
    workflow::frmwork::Status GetAssignTasksByOwnerStatus(const std::vector<std::string>& taskIDs, const std::string& owner, const TaskStatus& status, const int& limit);

private:
    // ===== 私有实现 =====
    workflow::frmwork::Status getTaskCount(const std::string& taskType, const std::string& pos, const std::vector<TaskStatus>& statusSet, int& count);
    void row2TTask(TTask& task, const drogon::orm::Row& r);
};
};
}; 