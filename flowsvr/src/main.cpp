#include <drogon/drogon.h>

#include <google/protobuf/message.h>
#include <google/protobuf/util/json_util.h>
#include <google/protobuf/text_format.h>

#include <future>
#include <iostream>
#include "proto/api.pb.h"
#include "frmwork/api.h"
#include "ctrl/api.h"


using namespace drogon;
using namespace drogon_model::workflow;
using namespace google::protobuf::util;
using namespace workflow::frmwork;
using namespace workflow::db;
using namespace workflow::flowsvr;



int main(int argc, char* argv[]) {
    std::srand(std::time(nullptr));

    std::string configPath = "config.json";
    if (argc > 1) {
        configPath = argv[1];
    }

    RegisterHandler<CreateTaskCtrl>(app(), "/create_task");
    RegisterHandler<GetTaskCtrl>(app(), "/get_task");
    RegisterHandler<GetTasksListCtrl>(app(), "/get_task_list");
    RegisterHandler<GetTaskScheduleCfgListCtrl>(app(), "/get_task_schedule_cfg_list");
    RegisterHandler<HoldTasksCtrl>(app(), "/hold_tasks");
    RegisterHandler<SetTaskCtrl>(app(), "/set_task");
    drogon::app().loadConfigFile(configPath);
    drogon::app().run();
    return 0;
}