#pragma once

#include "dao.h"


namespace workflow {
namespace db {
using namespace drogon_model::workflow;

class ScheduleCfgDao : public Dao {
public:
    ScheduleCfgDao();
    workflow::frmwork::Status Create(TScheduleCfg& cfg);
    workflow::frmwork::Status Save(TScheduleCfg& cfg);
    workflow::frmwork::Status Get(const std::string& taskType, TScheduleCfg& cfg);
    workflow::frmwork::Status GetList(std::vector<TScheduleCfg>& cfgs);
};
}
}