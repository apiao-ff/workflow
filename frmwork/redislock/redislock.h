#pragma once
#include <string>
#include <hiredis/hiredis.h>

class RedisLock {
public:
    RedisLock(const std::string& address);
    ~RedisLock();
    bool lock(const std::string& key, int ttl_ms = 3000);
    void unlock(const std::string& key);

private:

    std::string host_;
    int port_;
    std::string username_;
    std::string password_;
    redisContext* ctx_;
    std::string value_;
    bool locked_;

    void parseAddress(const std::string& address);
};
