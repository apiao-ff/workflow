#include "get_tasks_list.h"

using namespace drogon;
using namespace workflow::frmwork;
using namespace workflow::db;
using namespace workflow::flowsvr;

const int32_t GetTasksListHandler::MAX_TASK_LIST_LIMIT = 1000;
const int32_t GetTasksListHandler::DEFAULT_TASK_LIST_LIMIT = 1000;

Status GetTasksListHandler::HandleInput(std::shared_ptr<api::GetTaskListReq>& reqBody) {
    if (reqBody->task_type() == "") {
        LOG_ERROR << "task type is empty";
        return InputInvalid;
    }
    if (!TaskDao::IsValidStatus(reqBody->status())) {
        return InputInvalid;
    }
    return Status::OK;
}

Status GetTasksListHandler::HandleProcess(std::shared_ptr<api::GetTaskListReq>& reqBody, api::GetTaskListRsp& rspBody) {
    // 1. 初始化 DAO 及调度位置结构体
    // 2. 解析请求字段（任务类型、状态、limit），并限制最大查询数
    // 3. 查询调度起始位置（用于获取调度分片）
    // 4. 查询指定任务类型和状态的任务列表
    // 5. 遍历结果，按重试间隔和时间戳过滤部分任务后填充返回结构
    TaskDao taskDao;
    SchedulePosDao posDao;
    TSchedulePos schPos;
    std::vector<TTask> vecTasks;
    std::string taskType = reqBody->task_type();
    std::string taskTableName = GetTaskTableName(taskType);
    int limit = reqBody->limit();
    if (limit > MAX_TASK_LIST_LIMIT) {
        limit = MAX_TASK_LIST_LIMIT;
    }
    if (limit == 0) {
        limit = DEFAULT_TASK_LIST_LIMIT;
    }

    Status status = posDao.Get(taskTableName, schPos);
    if (!status.ok()) {
        return Status::FAIL;
    }

    std::string beginSchPos = std::to_string(schPos.getValueOfScheduleBeginPos());
    status = taskDao.GetTaskList(taskType, beginSchPos, TaskStatus(reqBody->status()), limit, vecTasks);
    if (!status.ok()) {
        return Status::FAIL;
    }

    for (auto& task : vecTasks) {
        if (task.getValueOfCrtRetryNum() != 0 && task.getValueOfMaxRetryInterval() != 0 &&
            task.getValueOfOrderTime() > TimestampNow()) {
            continue;
        }
        api::TaskData* taskData = rspBody.add_task_list();
        FillPBTaskModel(task, *taskData);
    }

    return Status::OK;
}