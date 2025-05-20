#pragma once


#include <mutex>
#include "task.h"

namespace workflow {
namespace worker {

class TaskMgr {
public:
    TaskMgr(const std::string& taskType, const std::string& svtHost, const std::string& redisHost, TaskBaseFactory factory);
    void Init();
    void Loop();
    void Hold(const api::TaskScheduleCfg& cfg, std::vector<TaskPtr>& tasks);
    static void RunTask(const api::TaskScheduleCfg& cfg, TaskPtr taskPtr);

private:
    void loadCfg();
    void keepLoadCfg();
    void schedule(api::TaskScheduleCfg scheduleCfg);

private:
    RedisLock redisLock_;
    Threadpool threadPool_;
    std::string taskType_;
    std::string taskSvrHost_;
    std::mutex lock_;
    std::map<std::string, api::TaskScheduleCfg> cfgMap_;
    bool stop_;
    TaskBaseFactory factory_;
};

}; 
}; 