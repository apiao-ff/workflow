#include "lark.h"


using namespace workflow::worker;
using namespace workflow::frmwork;

Lark::Lark(const api::TaskData& data, const std::string& host) : TaskBase(data, host) {

}

Status Lark::ContextLoad() {
    return Status::OK;
}

Status Lark::HandleProcess() {
    // 格式化时间为字符串
    auto format_time = [](const std::chrono::system_clock::time_point& tp) -> std::string {
        std::time_t t = std::chrono::system_clock::to_time_t(tp);
        std::tm tm = *std::localtime(&t);
        std::ostringstream oss;
        oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
        return oss.str();
        };
    auto start = std::chrono::system_clock::now();
    std::this_thread::sleep_for(std::chrono::seconds(5));
    auto end = std::chrono::system_clock::now();
    std::string start_time_str = format_time(start);
    std::string end_time_str = format_time(end);
    std::ostringstream json_oss;
    json_oss << "{\"start\": \"" << start_time_str << "\", \"end\": \"" << end_time_str << "\"}";
    std::string time_json = json_oss.str();
    this->TaskData().set_schedule_log(time_json);
    this->TaskData().set_task_context("done!");
    this->TaskData().set_status(Succ);
    return Status::OK;
}

Status Lark::handleFailedMust() {
    return Status::OK;
}

Status Lark::HandleFinishError() {
    return Status::OK;
}

void Lark::HandleFinish() {

};

TaskPtr Lark::CreateLark(const api::TaskData& data, const std::string& host) {
    return std::make_shared<Lark>(data, host);
}
