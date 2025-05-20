cmake_minimum_required(VERSION 3.16)
project(workflow)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(
    DOWNLOAD
    https://github.com/cpm-cmake/CPM.cmake/releases/download/v0.40.8/CPM.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake
    EXPECTED_HASH SHA256=78ba32abdf798bc616bab7c73aac32a17bbd7b06ad9e26a6add69de8f3ae4791
)
include(${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake)

CPMAddPackage(
    NAME drogon
    VERSION 1.9.0
    GITHUB_REPOSITORY drogonframework/drogon
    GIT_TAG v1.9.0
    OPTIONS
        "BUILD_EXAMPLES FALSE"
)

find_package(Protobuf REQUIRED)
ADD_SUBDIRECTORY(proto)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE_DIRECTORIES(${Protobuf_INCLUDE_DIRS})

set(FRMWORK_SOURCES
    frmwork/comm/comm.cpp
    frmwork/status/status.cpp
    frmwork/rpc/rpc.cpp
)

set(FLOWSVR_SOURCES
    flowsvr/src/model/TLarkTask1.cc
    flowsvr/src/model/TScheduleCfg.cc
    flowsvr/src/model/TSchedulePos.cc
    flowsvr/src/model/TTask.cc

    flowsvr/src/dao/dao.cpp
    flowsvr/src/dao/schedule_pos.cpp
    flowsvr/src/dao/schedule_cfg.cpp
    flowsvr/src/dao/task.cpp

    flowsvr/src/ctrl/ctrl.cpp
    flowsvr/src/ctrl/create_task.cpp
    flowsvr/src/ctrl/set_task.cpp
    flowsvr/src/ctrl/get_task.cpp
    flowsvr/src/ctrl/get_tasks_list.cpp
    flowsvr/src/ctrl/get_task_schedule_cfg_list.cpp
    flowsvr/src/ctrl/hold_tasks.cpp
)


set(WORKER_SOURCES
    worker/src/sdk/schedule.cpp
    worker/src/sdk/task.cpp
    worker/src/tasks/lark/lark.cpp
    frmwork/redislock/redislock.cpp
)

add_executable(flowsvr flowsvr/src/main.cpp ${FLOWSVR_SOURCES} ${FRMWORK_SOURCES})
target_link_libraries(flowsvr drogon proto ${Protobuf_LIBRARIES})

add_executable(worker worker/src/main.cpp ${WORKER_SOURCES}  ${FRMWORK_SOURCES})
target_link_libraries(worker drogon proto ${Protobuf_LIBRARIES})