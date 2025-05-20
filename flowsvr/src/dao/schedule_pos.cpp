#include "schedule_pos.h"

using namespace workflow::db;
using namespace workflow::frmwork;
using namespace drogon::orm;


SchedulePosDao::SchedulePosDao() {};


Status SchedulePosDao::Create(TSchedulePos& pos) {
    try {
        drogon::orm::Mapper<TSchedulePos> mp(clientPtr_);
        mp.insert(pos);
    }
    catch (const drogon::orm::DrogonDbException& e) {
        LOG_FATAL << "error:" << e.base().what();
        return DBExecErr;
    }
    return Status::OK;
}

Status SchedulePosDao::Save(TSchedulePos& pos) {
    try {
        drogon::orm::Mapper<TSchedulePos> mp(clientPtr_);
        mp.update(pos);
    }
    catch (const drogon::orm::DrogonDbException& e) {
        LOG_FATAL << "error:" << e.base().what();
        return DBExecErr;
    }
    return Status::OK;
}

Status SchedulePosDao::Get(const std::string& taskSetName, TSchedulePos& pos) {
    try {
        drogon::orm::Mapper<TSchedulePos> mp(clientPtr_);
        auto tasks = mp.limit(1).findBy(Criteria(TSchedulePos::Cols::_task_type, CompareOperator::EQ, taskSetName));
        if (tasks.size() == 0) {
            return ResourceNotFound;
        }
        else {
            pos = tasks[0];
        }
    }
    catch (const drogon::orm::DrogonDbException& e) {
        LOG_FATAL << "error:" << e.base().what();
        return DBExecErr;
    }
    return Status::OK;
}

Status SchedulePosDao::GetRandomSchedulePos(const std::string& taskSetName, int& pos) {
    TSchedulePos taskPos;
    Status status = Get(taskSetName, taskPos);
    if (!status.ok()) {
        return status;
    }
    int base = taskPos.getValueOfScheduleEndPos() - taskPos.getValueOfScheduleBeginPos() + 1;
    pos = (std::rand() % base) + taskPos.getValueOfScheduleBeginPos();
    return Status::OK;
}

Status SchedulePosDao::GetBeginSchedulePos(const std::string& taskSetName, int& pos) {
    TSchedulePos taskPos;
    Status status = Get(taskSetName, taskPos);
    if (!status.ok()) {
        return status;
    }
    pos = taskPos.getValueOfScheduleBeginPos();
    return Status::OK;
}

Status SchedulePosDao::GetNextPos(const std::string& pos, std::string& nextPos) {
    int posInt = std::stoi(nextPos);
    nextPos = std::to_string(posInt + 1);
    return Status::OK;
}

Status SchedulePosDao::GetPosList(std::vector<TSchedulePos>& vec) {
    try {
        drogon::orm::Mapper<TSchedulePos> mp(clientPtr_);
        vec = mp.findAll();
    }
    catch (const drogon::orm::DrogonDbException& e) {
        LOG_FATAL << "error:" << e.base().what();
    }
    return Status::OK;
}