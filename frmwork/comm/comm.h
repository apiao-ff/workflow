#pragma once

#include <memory>
#include <drogon/drogon.h>
#include <google/protobuf/message.h>
#include <google/protobuf/util/json_util.h>
#include <google/protobuf/text_format.h>
#include <string>
#include <stdexcept>
#include <random>
#include <sstream>
#include <vector>
#include <map>
#include <functional>
#include <list>

namespace Protobuf = google::protobuf;
namespace ProtobufUtil = google::protobuf::util;

using MessagePtr = std::shared_ptr<Protobuf::Message>;

template <typename... Args>
std::string Sprintf(const std::string& format, Args... args) {
    int size_s = std::snprintf(nullptr, 0, format.c_str(), args...) + 1; // Extra space for '\0'
    if (size_s <= 0) {
        throw std::runtime_error("Error during formatting.");
    }
    auto size = static_cast<size_t>(size_s);
    std::unique_ptr<char[]> buf(new char[size]);
    std::snprintf(buf.get(), size, format.c_str(), args...);
    return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
}

template <typename T>
void VectorJoin(const std::vector<T>& v, char c, std::string& s) {
    s.clear();
    for (typename std::vector<T>::const_iterator p = v.begin(); p != v.end(); ++p) {
        s += *p;
        if (p != v.end() - 1) {
            s += c;
        }
    }
}

void ReplaceString(std::string& subject, const std::string& search, const std::string& replace);

void StringSplit(const std::string& s, std::vector<std::string>& sv, const char delim = ' ');

void StringJoin(const std::vector<std::string>& v, char c, std::string& s);

uint32_t TimestampNow();

// 生成符合 UUID version 4 格式的唯一标识符：
// 8-4-4(首'4')-4(首[8, 9, a, b])-12
// 格式为：xxxxxxxx-xxxx-4xxx-Nxxx-xxxxxxxxxxxx
// 说明：
// - 共 128 位（16 字节），用 32 个十六进制字符 + 4 个连字符组成
// - 第 3 段首字符固定为 '4'：表示 UUID version 4（随机型）
// - 第 4 段首字符为 [8, 9, a, b]：符合 RFC 4122 variant 规则（高两位为 '10'）
// - 其余字符均为随机十六进制数（0~f）
// 用于生成唯一的任务 ID、请求 ID 等
std::string generate_uuid_v4();