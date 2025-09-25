#pragma once

#include "req_context.h"
#include <boost/beast/http.hpp>
#include <functional>
#include <memory>

namespace http = boost::beast::http;

class Middleware
{
public:
    virtual ~Middleware() = default;
    virtual void operator()(const ReqContext &ctxt, http::response<http::string_body> &res,
                            std::function<void()> next) = 0;
};
typedef std::shared_ptr<Middleware> MiddlewarePtr;