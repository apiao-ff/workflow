#pragma once

#include <map>
#include "frmwork/status/status.h"

namespace workflow {
namespace frmwork {
// 状态码
enum Code
{
    SUCCESS = 1,
    ERR_INPUT_INVALID = 8020,
    ERR_SHOULD_BIND = 8021,
    ERR_JSON_MARSHAL = 8022,
    ERR_GET_TASK_INFO = 8035,
    ERR_GET_TASK_HANDLE_PROCESS = 8036,
    ERR_CREATE_TASK = 8037,
    ERR_GET_TASK_LIST_FROM_DB = 8038,
    ERR_GET_TASK_SET_POS_FROM_DB = 8039,
    ERR_INCREASE_CRT_RETRY_NUM = 8040,
    ERR_SET_TASK = 8041,
    ERR_GET_TASK_POS = 8042,
    ERR_GET_PROCESSING_COUNT = 8043,
    ERR_SET_USER_PRIORITY = 8045,
    ERR_GET_TASK_CFG_FROM_DB = 8039,
    ERR_DB_EXEC_ERR = 9000,
    ERR_RESOURCE_NOT_FOUND = 9001,
    ERR_HTTP_REQUEST_ERR = 9002,
};

// 状态码消息映射
static std::map<Code, std::string> ErrMsg = {
    {SUCCESS, "ok"},
    {ERR_INPUT_INVALID, "input invalid"},
    {ERR_SHOULD_BIND, "should bind failed"},
    {ERR_JSON_MARSHAL, "json marshal failed"},
    {ERR_GET_TASK_INFO, "get task info failed"},
    {ERR_GET_TASK_HANDLE_PROCESS, "get task handle process Failed"},
    {ERR_CREATE_TASK, "create task failed"},
    {ERR_GET_TASK_LIST_FROM_DB, "get task list from db failed"},
    {ERR_GET_TASK_SET_POS_FROM_DB, "get task set pos from db failed"},
    {ERR_SET_TASK, "set task failed"},
    {ERR_INCREASE_CRT_RETRY_NUM, "increase crt retry num failed"},
    {ERR_GET_TASK_POS, "get task pos failed"},
    {ERR_GET_PROCESSING_COUNT, "get processing count failed"},
    {ERR_SET_USER_PRIORITY, "set user priority failed"},
    {ERR_GET_TASK_CFG_FROM_DB, "get task cfg failed"},
};


const Status InputInvalid = Status(ERR_INPUT_INVALID, "input invalid");
const Status JsonMarshalErr = Status(ERR_JSON_MARSHAL, "should bind failed");
const Status DBExecErr = Status(ERR_DB_EXEC_ERR, "db exec err");
const Status ResourceNotFound = Status(ERR_RESOURCE_NOT_FOUND, "resource not found");


};
};
