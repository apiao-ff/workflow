#include "worker/src/sdk/schedule.h"
#include "worker/src/tasks/lark/lark.h"
#include <stdlib.h>
#include <unistd.h>
using namespace workflow::worker;

int main(int argc, char* argv[]) {
    std::string configPath = "config.json";
    if (argc > 1) {
        configPath = argv[1];
    }
    drogon::app().loadConfigFile(configPath);
    std::string type = drogon::app().getCustomConfig()["task_type"].asString();
    std::string svr = drogon::app().getCustomConfig()["flowsvr_host"].asString();
    std::string redis = drogon::app().getCustomConfig()["redis_host"].asString();
    std::thread t(
        [&]() {
            TaskMgr mgr(type, svr, redis, Lark::CreateLark);
            mgr.Init();
            mgr.Loop();
        }
    );
    drogon::app().run();
    // 
    // 
    return 0;
}