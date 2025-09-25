#include "query_task_result_handler.h"
#include <boost/json.hpp>

void query_task_result_handler::handle_request(const ReqContext &ctx, http::response<http::string_body> &res)
{
    res.result(http::status::ok);
    res.set(http::field::content_type, "application/json");
    boost::json::object obj;
    std::string taskId = ctx.path_param("taskId");

    res.body() = boost::json::serialize(obj);
    res.prepare_payload();
}