#pragma once

#include "dao.h"

namespace workflow {
namespace db {

using namespace drogon_model::workflow;

class SchedulePosDao : public Dao {
public:
    SchedulePosDao();
    workflow::frmwork::Status Create(TSchedulePos& pos);
    workflow::frmwork::Status Save(TSchedulePos& pos);
    workflow::frmwork::Status Get(const std::string& taskSetName, TSchedulePos& pos);
    workflow::frmwork::Status GetRandomSchedulePos(const std::string& taskSetName, int& pos);
    workflow::frmwork::Status GetBeginSchedulePos(const std::string& taskSetName, int& pos);
    workflow::frmwork::Status GetNextPos(const std::string& pos, std::string& nextPos);
    workflow::frmwork::Status GetPosList(std::vector<TSchedulePos>& vec);
};
}; 
}; 