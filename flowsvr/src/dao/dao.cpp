#include "dao.h"

using namespace workflow::db;

Dao::Dao() {
    clientPtr_ = drogon::app().getDbClient("db");
}

Dao::~Dao() {}

std::string GetTaskTableName(const std::string& taskType) {
    return Sprintf("t_%s_task", taskType.c_str());
}


