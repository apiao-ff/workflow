#pragma once

#include <string>

namespace workflow
{
namespace frmwork
{
enum error
{
    OK,
    FAILED,
};

class Status
{
public:
    Status();
    Status(const Status& x);
    Status(int error_code, std::string error_message);
    ~Status();

public:
    static const Status OK;
    static const Status FAIL;

public:
    void set(int code, std::string msg);
    bool ok() const;
    int error_code() const;
    std::string error_message() const;

    bool operator==(const Status& x) const;
    bool operator!=(const Status& x) const;

private:
    int error_code_;
    std::string error_message_;
};
}
};