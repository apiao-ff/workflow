#include "schedule_cfg.h"

using namespace workflow::db;
using namespace workflow::frmwork;
using namespace drogon::orm;

ScheduleCfgDao::ScheduleCfgDao() {};


Status ScheduleCfgDao::Create(TScheduleCfg& cfg) {
    try {
        drogon::orm::Mapper<TScheduleCfg> mp(clientPtr_);
        mp.insert(cfg);
    }
    catch (const drogon::orm::DrogonDbException& e) {
        LOG_FATAL << "error:" << e.base().what();
        return DBExecErr;
    }
    return Status::OK;
}

Status ScheduleCfgDao::Save(TScheduleCfg& cfg) {
    try {
        drogon::orm::Mapper<TScheduleCfg> mp(clientPtr_);
        mp.update(cfg);
    }
    catch (const drogon::orm::DrogonDbException& e) {
        LOG_FATAL << "error:" << e.base().what();
        return DBExecErr;
    }
    return Status::OK;
}

Status ScheduleCfgDao::Get(const std::string& taskType, TScheduleCfg& cfg) {
    try {
        drogon::orm::Mapper<TScheduleCfg> mp(clientPtr_);
        auto cfgs = mp.limit(1).findBy(Criteria(TScheduleCfg::Cols::_task_type, CompareOperator::EQ, taskType));
        if (cfgs.empty()) {
            LOG_FATAL << "error cfgs size = 0";
            return ResourceNotFound;
        }
        else {
            cfg = cfgs[0];
        }
    }
    catch (const drogon::orm::DrogonDbException& e) {
        LOG_FATAL << "error:" << e.base().what();
        return DBExecErr;
    }
    return Status::OK;
}

Status ScheduleCfgDao::GetList(std::vector<TScheduleCfg>& cfgs) {
    try {
        drogon::orm::Mapper<TScheduleCfg> mp(clientPtr_);
        cfgs = mp.findAll();
    }
    catch (const drogon::orm::DrogonDbException& e) {
        LOG_FATAL << "error:" << e.base().what();
        return DBExecErr;
    }
    return Status::OK;
}