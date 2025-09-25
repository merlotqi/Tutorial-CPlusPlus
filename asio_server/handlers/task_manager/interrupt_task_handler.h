#pragma once

#include <router.h>
#include <session.h>

class interrupt_task_handler : public ApiHandler
{
public:
    void handle_request(const ReqContext &ctx, http::response<http::string_body> &res) override;
};
REGISTER_DYNAMIC_HANDLER("/interruptTask/{taskId}", interrupt_task_handler)

class interrupt_task_list_handler : public ApiHandler
{
public:
    void handle_request(const ReqContext &ctx, http::response<http::string_body> &res) override;
};
REGISTER_STATIC_HANDLER("/interruptTaskList", interrupt_task_list_handler)