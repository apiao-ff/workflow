#include "ctrl.h"



// proto::task -> model::task 缺失 任务类型，任务ID，创建时间，修改时间
void FillDBTaskModel(const api::TaskData& taskdata, workflow::db::TTask& ttask) {
    ttask.setUserId(taskdata.user_id());

    ttask.setStatus(taskdata.status());
    ttask.setTaskStage(taskdata.task_stage());

    ttask.setScheduleLog(taskdata.schedule_log());
    ttask.setTaskContext(taskdata.task_context());

    ttask.setCrtRetryNum(taskdata.crt_retry_num());
    ttask.setMaxRetryNum(taskdata.max_retry_num());
    ttask.setMaxRetryInterval(taskdata.max_retry_interval());

    ttask.setPriority(taskdata.priority());
    ttask.setOrderTime(taskdata.order_time());
}

// model::task -> proto::task 全字段
void FillPBTaskModel(const workflow::db::TTask& ttask, api::TaskData& taskdata) {
    taskdata.set_user_id(ttask.getValueOfUserId());
    taskdata.set_task_id(ttask.getValueOfTaskId());
    taskdata.set_task_type(ttask.getValueOfTaskType());

    taskdata.set_status(ttask.getValueOfStatus());
    taskdata.set_task_stage(ttask.getValueOfTaskStage());

    taskdata.set_schedule_log(ttask.getValueOfScheduleLog());
    taskdata.set_task_context(ttask.getValueOfTaskContext());

    taskdata.set_crt_retry_num(ttask.getValueOfCrtRetryNum());
    taskdata.set_max_retry_num(ttask.getValueOfMaxRetryNum());
    taskdata.set_max_retry_interval(ttask.getValueOfMaxRetryInterval());

    taskdata.set_priority(ttask.getValueOfPriority());
    taskdata.set_order_time(ttask.getValueOfOrderTime());

    taskdata.set_create_time(ttask.getValueOfCreateTime().toFormattedString(false));
    taskdata.set_modify_time(ttask.getValueOfModifyTime().toFormattedString(false));
}

void FillDBScheduleCfgModel(const workflow::db::TScheduleCfg& tcfg, api::TaskScheduleCfg& cfg) {
    cfg.set_task_type(tcfg.getValueOfTaskType());
    cfg.set_schedule_limit(tcfg.getValueOfScheduleLimit());
    cfg.set_schedule_interval(tcfg.getValueOfScheduleInterval());
    cfg.set_max_processing_time(tcfg.getValueOfMaxProcessingTime());
    cfg.set_max_retry_num(tcfg.getValueOfMaxRetryNum());
    cfg.set_max_retry_interval(tcfg.getValueOfRetryInterval());
    cfg.set_create_time(tcfg.getValueOfCreateTime().toFormattedString(false));
    cfg.set_modify_time(tcfg.getValueOfModifyTime().toFormattedString(false));
}