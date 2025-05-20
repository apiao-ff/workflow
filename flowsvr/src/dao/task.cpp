#include "task.h"

using namespace workflow::db;
using namespace workflow::frmwork;
using namespace drogon::orm;

TaskDao::TaskDao() {

}

// ===== 工具函数 =====
// 判断状态是否有效
bool TaskDao::IsValidStatus(int32_t status) {
    if (status > Failed || status < 0) {
        return false;
    }
    return true;
}

// 从 task_id 中解析出对应的 taskType 和 pos（用于反推出表名）
void TaskDao::GetTablePosFromTaskID(const std::string& taskID, std::string& taskType, std::string& pos) {
    std::vector<std::string> splitStr;
    StringSplit(taskID, splitStr, '_');
    if (splitStr.size() < 3) {
        LOG_ERROR << "task id invalid" << taskID;
    }
    else {
        taskType = splitStr[1];
        pos = splitStr[2];
    }
}


// t_<type>_task_<pos>
std::string TaskDao::GetTableName(const std::string& taskType, const std::string& pos) {
    return Sprintf("t_%s_task_%s", taskType.c_str(), pos.c_str());
}

// <uuid>_<taskType>_<pos>
std::string TaskDao::GenTaskID(const std::string& taskType, const std::string& pos) {
    std::string filterTaskType(taskType);
    ReplaceString(filterTaskType, "_", "-");
    return Sprintf("%s_%s_%s", generate_uuid_v4().c_str(), filterTaskType.c_str(), pos.c_str());
}

// 创建下一张任务表（待实现）
Status TaskDao::CreateNextTable(const std::string& taskType, const std::string& pos) {
    // todo
    return Status::OK;
}




// ===== 任务创建 =====
// 创建一条新任务记录，如果未指定 task_id，则自动生成，并写入指定表
Status TaskDao::Create(const std::string& taskType, const std::string& pos, TTask& task) {
    try {
        std::string tableName = GetTableName(taskType, pos);
        if (task.getValueOfTaskId() == "") {
            task.setTaskId(GenTaskID(taskType, pos));
        }
        task.setTaskType(taskType);
        auto f = clientPtr_->execSqlAsyncFuture(
            "insert into " + tableName +
            " (user_id,"
            " task_id,"
            " task_type,"
            " task_stage,"
            " status,"
            " priority,"
            " crt_retry_num,"
            " max_retry_num,"
            " max_retry_interval,"
            " schedule_log,"
            " task_context,"
            " order_time)"
            " VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)",
            task.getValueOfUserId(),
            task.getValueOfTaskId(),
            task.getValueOfTaskType(),
            task.getValueOfTaskStage(),
            task.getValueOfStatus(),
            task.getValueOfPriority(),
            task.getValueOfCrtRetryNum(),
            task.getValueOfMaxRetryNum(),
            task.getValueOfMaxRetryInterval(),
            task.getValueOfScheduleLog(),
            task.getValueOfTaskContext(),
            task.getValueOfOrderTime());
        auto result = f.get();
        LOG_INFO << "create task " << result.size() << " rows affected";
    }
    catch (const drogon::orm::DrogonDbException& e) {
        LOG_FATAL << "error:" << e.base().what();
    }
    return Status::OK;
}




// ===== 查询接口 =====
// 根据 task_id 查找并返回任务记录
Status TaskDao::Find(const std::string& taskID, TTask& task) {
    std::string taskType, pos, tableName;
    GetTablePosFromTaskID(taskID, taskType, pos);
    tableName = GetTableName(taskType, pos);
    try {
        auto f = clientPtr_->execSqlAsyncFuture("select * from " + tableName + " where task_id = ?", taskID);
        auto result = f.get();
        if (result.size() == 0) {
            return ResourceNotFound;
        }
        row2TTask(task, result[0]);
    }
    catch (const DrogonDbException& e) {
        LOG_ERROR << "error:" << e.base().what();
        return DBExecErr;
    }
    return Status::OK;
}

// 获取指定 task_type 和 pos 对应表中、指定状态的最多 limit 条任务记录
Status TaskDao::GetTaskList(const std::string& taskType, const std::string& pos, const TaskStatus& status, int limit, std::vector<TTask>& tasklist) {
    std::string tableName = GetTableName(taskType, pos);
    try {
        auto f = clientPtr_->execSqlAsyncFuture(
            "select * from " + tableName +
            " where task_type = ? and status = ? order by order_time desc limit " + std::to_string(limit),
            taskType, status
        );
        auto result = f.get();
        if (result.size() == 0) {
            return ResourceNotFound;
        }
        LOG_INFO << "task list len: " << result.size();
        for (auto row : result) {
            tasklist.push_back(TTask());
            row2TTask(tasklist.back(), row);
        }
    }
    catch (const DrogonDbException& e) {
        LOG_ERROR << "error:" << e.base().what();
        return DBExecErr;
    }
    return Status::OK;
}

// 获取状态为 Pending 或 Processing 的任务列表
Status TaskDao::GetAliveTaskList(const std::string& taskType, const std::string& pos, const TaskStatus& status, int limit, std::vector<TTask>& tasklist) {
    std::string tableName = GetTableName(taskType, pos);
    try {
        auto f = clientPtr_->execSqlAsyncFuture(
            "select * from " + tableName +
            " where status in (?, ?) order by modify_time desc limit " + std::to_string(limit),
            Pending, Processing
        );
        auto result = f.get();
        if (result.size() == 0) {
            return ResourceNotFound;
        }
        for (auto row : result) {
            tasklist.push_back(TTask());
            row2TTask(tasklist.back(), row);
        }
    }
    catch (const DrogonDbException& e) {
        LOG_ERROR << "error:" << e.base().what();
        return DBExecErr;
    }
    return Status::OK;
}

// 获取处于 Pending 或 Processing 状态的任务数量
Status TaskDao::GetAliveTaskCount(const std::string& taskType, const std::string& pos, int& count) {
    return getTaskCount(taskType, pos, std::vector<TaskStatus>{Pending, Processing}, count);
}

// 获取所有状态（Pending、Processing、Succ、Failed）任务的数量
Status TaskDao::GetAllTaskCount(const std::string& taskType, const std::string& pos, int& count) {
    return getTaskCount(taskType, pos, std::vector<TaskStatus>{Pending, Processing, Succ, Failed}, count);
}




// ===== 修改接口 =====
// 设置指定任务的状态
Status TaskDao::SetStatus(const std::string& taskID, const TaskStatus& status) {
    std::string taskType, pos, tableName;
    GetTablePosFromTaskID(taskID, taskType, pos);
    tableName = GetTableName(taskType, pos);
    try {
        auto f = clientPtr_->execSqlAsyncFuture(
            "update " + tableName + " set status = ? where task_id = ?",
            status, taskID
        );
        auto result = f.get();
        if (result.size() == 0) {
            return ResourceNotFound;
        }
    }
    catch (const DrogonDbException& e) {
        LOG_ERROR << "error:" << e.base().what();
        return DBExecErr;
    }
    return Status::OK;
}

// 批量更新多个任务 ID 的任务状态
Status TaskDao::BatchSetStatus(const std::vector<std::string>& taskIDList, const TaskStatus& status) {
    if (taskIDList.size() == 0) {
        return Status::OK;
    }
    std::vector<std::string> quoteTaskIDList;
    std::string taskType, pos, tableName, taskInCond;
    GetTablePosFromTaskID(taskIDList[0], taskType, pos);
    tableName = GetTableName(taskType, pos);
    for (auto taskID : taskIDList) {
        quoteTaskIDList.push_back("'" + taskID + "'");
    }
    StringJoin(quoteTaskIDList, ',', taskInCond);
    try {
        auto f = clientPtr_->execSqlAsyncFuture(
            "update " + tableName + " set status = ? where task_id in ( " + taskInCond + " )",
            status
        );
        auto result = f.get();
    }
    catch (const DrogonDbException& e) {
        LOG_ERROR << "error:" << e.base().what();
        return DBExecErr;
    }
    return Status::OK;
}

// 设置指定任务的上下文内容（task_context 字段）
Status TaskDao::SetContext(const std::string& taskID, const std::string& context) {
    std::string taskType, pos, tableName;
    GetTablePosFromTaskID(taskID, taskType, pos);
    tableName = GetTableName(taskType, pos);
    try {
        auto f = clientPtr_->execSqlAsyncFuture(
            "update " + tableName + " set task_context = ? where task_id = ?", context, taskID
        );
        auto result = f.get();
        if (result.size() == 0) {
            return ResourceNotFound;
        }
    }
    catch (const DrogonDbException& e) {
        LOG_ERROR << "error:" << e.base().what();
        return DBExecErr;
    }
    return Status::OK;
}

// 更新指定任务记录（通过 task_id 匹配）并重设 order_time
Status TaskDao::Save(TTask& task) {
    std::string taskType, pos, tableName;
    GetTablePosFromTaskID(task.getValueOfTaskId(), taskType, pos);
    if (taskType == "" || pos == "") {
        return InputInvalid;
    }
    tableName = GetTableName(taskType, pos);
    try {
        task.setOrderTime(TimestampNow());
        auto f = clientPtr_->execSqlAsyncFuture(
            "update " + tableName +
            " set user_id = ?,"
            " task_type = ?,"
            " task_stage = ?,"
            " status = ?,"
            " priority = ?,"
            " crt_retry_num = ?,"
            " max_retry_num = ?,"
            " max_retry_interval = ?,"
            " schedule_log = ?,"
            " task_context = ?,"
            " order_time = ?"
            " where task_id = ?",
            task.getValueOfUserId(),
            task.getValueOfTaskType(),
            task.getValueOfTaskStage(),
            task.getValueOfStatus(),
            task.getValueOfPriority(),
            task.getValueOfCrtRetryNum(),
            task.getValueOfMaxRetryNum(),
            task.getValueOfMaxRetryInterval(),
            task.getValueOfScheduleLog(),
            task.getValueOfTaskContext(),
            task.getValueOfOrderTime(),
            task.getValueOfTaskId());
        auto result = f.get();
        LOG_INFO << "update task " << result.size() << " rows affected";
    }
    catch (const drogon::orm::DrogonDbException& e) {
        LOG_FATAL << "error:" << e.base().what();
    }
    return Status::OK;
}




// ===== 异常处理/特殊流程 =====
// 获取处理时间超过 maxProcessTime 的任务（状态为 Processing）
Status TaskDao::GetLongTimeProcessing(const std::string& taskType, const std::string& pos, const int& maxProcessTime, const int& limit, std::vector<TTask>& tasklist) {
    std::string tableName = GetTableName(taskType, pos);
    try {
        auto f = clientPtr_->execSqlAsyncFuture(
            "select * from " + tableName +
            " where status = ? and unix_timestamp(modify_time) + ? < ? limit ?",
            Processing, maxProcessTime, limit
        );
        auto result = f.get();
        if (result.size() == 0) {
            return ResourceNotFound;
        }
        for (auto row : result) {
            tasklist.push_back(TTask());
            row2TTask(tasklist.back(), row);
        }
    }
    catch (const DrogonDbException& e) {
        LOG_ERROR << "error:" << e.base().what();
        return DBExecErr;
    }
    return Status::OK;
}

// 将长时间未完成的 Processing 任务重置为 Pending 状态
Status TaskDao::ModifyTimeoutPending(const std::string& taskType, const std::string& pos, const int& maxProcessTime) {
    std::string tableName = GetTableName(taskType, pos);
    try {
        auto f = clientPtr_->execSqlAsyncFuture(
            "update " + tableName +
            " set status = ? where status = ? and unix_timestamp(modify_time) + ? < ?",
            Pending, Processing, maxProcessTime, TimestampNow()
        );
        drogon::orm::Result result = f.get();
        LOG_INFO << "affected rows: " << result.affectedRows();
    }
    catch (const DrogonDbException& e) {
        LOG_ERROR << "error:" << e.base().what();
        return DBExecErr;
    }
    return Status::OK;
}

// 将指定任务的 crt_retry_num 字段 +1
Status TaskDao::IncreaseCrtRetryNum(const std::string& taskID) {
    std::string taskType, pos, tableName;
    GetTablePosFromTaskID(taskID, taskType, pos);
    tableName = GetTableName(taskType, pos);
    try {
        auto f = clientPtr_->execSqlAsyncFuture(
            "update " + tableName + " set crt_retry_num = crt_retry_num + 1 where task_id = ?",
            taskID
        );
        auto result = f.get();
        LOG_INFO << "affected rows: " << result.affectedRows();
    }
    catch (const DrogonDbException& e) {
        LOG_ERROR << "error:" << e.base().what();
        return DBExecErr;
    }
    return Status::OK;
}
// 设置指定任务的调度日志 schedule_log 字段
Status TaskDao::SetScheduleLog(const std::string& taskID, const std::string& log) {
    std::string taskType, pos, tableName;
    GetTablePosFromTaskID(taskID, taskType, pos);
    tableName = GetTableName(taskType, pos);
    try {
        auto f = clientPtr_->execSqlAsyncFuture(
            "update " + tableName + " set schedule_log = ? where task_id = ?",
            log, taskID
        );
        auto result = f.get();
        LOG_INFO << "affected rows: " << result.affectedRows();
    }
    catch (const DrogonDbException& e) {
        LOG_ERROR << "error:" << e.base().what();
        return DBExecErr;
    }
    return Status::OK;
}




// ===== 私有实现 =====
// 将 drogon 的 Row 数据转换成 TTask 对象
void TaskDao::row2TTask(TTask& task, const drogon::orm::Row& r) {
    task.setId(r["id"].as<int32_t>());
    task.setUserId(r["user_id"].as<std::string>());
    task.setTaskId(r["task_id"].as<std::string>());
    task.setTaskType(r["task_type"].as<std::string>());
    task.setTaskStage(r["task_stage"].as<std::string>());
    task.setStatus(r["status"].as<uint8_t>());
    task.setPriority(r["priority"].as<int32_t>());
    task.setCrtRetryNum(r["crt_retry_num"].as<int32_t>());
    task.setMaxRetryNum(r["max_retry_num"].as<int32_t>());
    task.setMaxRetryInterval(r["max_retry_interval"].as<int32_t>());
    task.setScheduleLog(r["schedule_log"].as<std::string>());
    task.setTaskContext(r["task_context"].as<std::string>());
    task.setOrderTime(r["order_time"].as<int32_t>());
    task.setCreateTime(trantor::Date::fromDbStringLocal(r["create_time"].as<std::string>()));
    task.setModifyTime(trantor::Date::fromDbStringLocal(r["modify_time"].as<std::string>()));
}

// 获取指定状态集下任务数量
Status TaskDao::getTaskCount(const std::string& taskType, const std::string& pos, const std::vector<TaskStatus>& statusSet, int& count) {
    std::string tableName = GetTableName(taskType, pos);
    std::string statusCond;
    VectorJoin<TaskStatus>(statusSet, ',', statusCond);
    try {
        auto f = clientPtr_->execSqlAsyncFuture(
            "select count(1) as cnt from " + tableName + " where status in (?)",
            statusCond
        );
        auto result = f.get();
        if (result.size() == 0) {
            return ResourceNotFound;
        }
        count = result[0]["cnt"].as<int>();
    }
    catch (const DrogonDbException& e) {
        LOG_ERROR << "error:" << e.base().what();
        return DBExecErr;
    }
    return Status::OK;
}
