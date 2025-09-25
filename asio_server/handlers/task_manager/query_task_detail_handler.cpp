#include "query_task_detail_handler.h"
#include <boost/json.hpp>

/* ------------------------ query_task_detail_handler ----------------------- */

void query_task_detail_handler::handle_request(const ReqContext &ctx, http::response<http::string_body> &res)
{
    res.result(http::status::ok);
    res.set(http::field::content_type, "application/json");

    boost::json::object obj;

    res.body() = boost::json::serialize(obj);
    res.prepare_payload();
}

/* --------------------- query_task_detail_list_handler --------------------- */

void query_task_detail_list_handler::handle_request(const ReqContext &ctx, http::response<http::string_body> &res)
{
    res.result(http::status::ok);
    res.set(http::field::content_type, "application/json");

    boost::json::object obj;
    res.body() = boost::json::serialize(obj);
    res.prepare_payload();
}