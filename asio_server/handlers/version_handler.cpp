#include "version_handler.h"
#include <boost/json.hpp>

void version_handler::handle_request(const ReqContext &ctx, http::response<http::string_body> &res)
{
    res.result(http::status::ok);
    res.set(http::field::content_type, "application/json");

    boost::json::object obj;
    obj["code"] = 0;
    obj["message"] = "success";
    obj["version"] = "1.0.0";

    res.body() = boost::json::serialize(obj);
    res.prepare_payload();
}