#pragma once

#include <router.h>
#include <session.h>

class query_task_detail_handler : public ApiHandler
{
public:
    void handle_request(const ReqContext &ctx, http::response<http::string_body> &res) override;
};
REGISTER_DYNAMIC_HANDLER("/taskDetail/{taskId}", query_task_detail_handler)

class query_task_detail_list_handler : public ApiHandler
{
public:
    void handle_request(const ReqContext &ctx, http::response<http::string_body> &res) override;
};
REGISTER_STATIC_HANDLER("/taskDetailList", query_task_detail_list_handler)