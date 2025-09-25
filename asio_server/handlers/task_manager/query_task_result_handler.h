#pragma once

#include <router.h>
#include <session.h>

class query_task_result_handler : public ApiHandler
{
public:
    void handle_request(const ReqContext &ctx, http::response<http::string_body> &res) override;
};
REGISTER_DYNAMIC_HANDLER("/queryTaskResult/{taskId}", query_task_result_handler)