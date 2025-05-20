#include "comm.h"
#include <ctime>
#include <chrono>
#include <sys/time.h>

void ReplaceString(std::string& subject, const std::string& search, const std::string& replace) {
    size_t pos = 0;
    while ((pos = subject.find(search, pos)) != std::string::npos) {
        subject.replace(pos, search.length(), replace);
        pos += replace.length();
    }
}

void StringSplit(const std::string& s, std::vector<std::string>& sv, const char delim) {
    sv.clear();
    std::istringstream iss(s);
    std::string temp;
    while (std::getline(iss, temp, delim)) {
        sv.emplace_back(std::move(temp));
    }
}

void StringJoin(const std::vector<std::string>& v, const char c, std::string& s) {
    s.clear();
    for (auto p = v.begin(); p != v.end(); ++p) {
        s += *p;
        if (p != v.end() - 1) {
            s += c;
        }
    }
}

uint32_t TimestampNow() {
    unsigned long int sec = time(NULL);
    return sec;
}

std::string generate_uuid_v4()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, 15);
    static std::uniform_int_distribution<> dis2(8, 11);

    std::stringstream ss;
    int i;
    ss << std::hex;
    for (i = 0; i < 8; i++) {
        ss << dis(gen);
    }

    ss << "-";
    for (i = 0; i < 4; i++) {
        ss << dis(gen);
    }

    ss << "-4";
    for (i = 0; i < 3; i++) {
        ss << dis(gen);
    }

    ss << "-";
    ss << dis2(gen);
    for (i = 0; i < 3; i++) {
        ss << dis(gen);
    }

    ss << "-";
    for (i = 0; i < 12; i++) {
        ss << dis(gen);
    };

    return ss.str();
}
