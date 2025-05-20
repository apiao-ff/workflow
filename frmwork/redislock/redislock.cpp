#include "redislock.h"

#include <chrono>
#include <stdexcept>
#include <sstream>

void RedisLock::parseAddress(const std::string& addressStr) {
    std::istringstream iss(addressStr);
    std::string hostPort, token;

    iss >> hostPort;

    // 拆 host:port
    auto pos = hostPort.find(':');
    if (pos == std::string::npos)
        throw std::invalid_argument("Invalid Redis host format");

    host_ = hostPort.substr(0, pos);
    port_ = std::stoi(hostPort.substr(pos + 1));

    // 解析额外参数
    while (iss >> token) {
        if (token.find("user:") == 0) {
            username_ = token.substr(5);
        }
        else if (token.find("pwd:") == 0) {
            password_ = token.substr(4);
        }
    }
}

RedisLock::RedisLock(const std::string& address)
    : ctx_(nullptr), locked_(false) {
    parseAddress(address);
    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
    ctx_ = redisConnectWithTimeout(host_.c_str(), port_, timeout);
    if (!ctx_ || ctx_->err) {
        throw std::runtime_error("Failed to connect to Redis: " + std::string(ctx_ ? ctx_->errstr : "null"));
    }
    if (!username_.empty()) {
        redisCommand(ctx_, "AUTH %s %s", username_.c_str(), password_.c_str());
    }

}

RedisLock::~RedisLock() {
    if (ctx_) {
        redisFree(ctx_);
        ctx_ = nullptr;
    }
}

bool RedisLock::lock(const std::string& key, int ttl_ms) {
    value_ = std::to_string(std::chrono::steady_clock::now().time_since_epoch().count());
    std::string cmd = "SET " + key + " " + value_ + " NX PX " + std::to_string(ttl_ms);
    redisReply* reply = (redisReply*)redisCommand(ctx_, cmd.c_str());
    if (reply && reply->type == REDIS_REPLY_STATUS && std::string(reply->str) == "OK") {
        locked_ = true;
    }
    if (reply) freeReplyObject(reply);
    return locked_;
}

void RedisLock::unlock(const std::string& key) {
    if (!locked_) return;
    const char* script = "if redis.call('get', KEYS[1]) == ARGV[1] then return redis.call('del', KEYS[1]) else return 0 end";
    redisReply* reply = (redisReply*)redisCommand(ctx_, "EVAL %s 1 %s %s", script, key.c_str(), value_.c_str());
    if (reply) freeReplyObject(reply);
    locked_ = false;
}
