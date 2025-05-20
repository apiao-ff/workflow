#include "status.h"

using namespace workflow::frmwork;

const Status Status::OK = Status(error::OK, "success");
const Status Status::FAIL = Status(error::FAILED, "failed");

Status::Status() {
    error_code_ = error::OK;
    error_message_ = "success";
}

Status::Status(const Status& x) {
    error_code_ = x.error_code();
    error_message_ = x.error_message();
}

Status::Status(int error_code, std::string error_message) : error_code_(error_code), error_message_(error_message) {

}

Status::~Status() {

}

void Status::set(int code, std::string msg) {
    error_code_ = code;
    error_message_ = msg;
}

bool Status::ok() const {
    return error_code_ == error::OK;
}

int Status::error_code() const {
    return error_code_;
}

std::string Status::error_message() const {
    return error_message_;
}

bool Status::operator==(const Status& x) const {
    return error_code_ == x.error_code();
}

bool Status::operator!=(const Status& x) const {
    return !operator==(x);
}
