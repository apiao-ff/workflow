#pragma once

#include <drogon/drogon.h>
#include <drogon/orm/CoroMapper.h>
#include <string>
#include <cstdlib>
#include <ctime>

#include "frmwork/api.h"
#include "flowsvr/src/model/api.h"

namespace workflow {
namespace db {

class Dao {
public:
    Dao();
    virtual ~Dao();

protected:
    drogon::orm::DbClientPtr clientPtr_;
};

}
}

std::string GetTaskTableName(const std::string& taskType);
