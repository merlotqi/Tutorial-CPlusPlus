#pragma once

#include "req_context.h"
#include <boost/beast/http.hpp>

namespace http = boost::beast::http;

class Middleware;
class ApiHandler
{
public:
    virtual ~ApiHandler() = default;
    void use(std::shared_ptr<Middleware> middleware);
    void
    use(std::function<void(const ReqContext &ctxt, http::response<http::string_body> &res, std::function<void()> next)>
                middleware_func);
    template<typename T, typename... Args>
    void use(Args &&...args)
    {
        middlewares_.push_back(std::make_shared<T>(std::forward<Args>(args)...));
    }

    void execute(const ReqContext &ctxt, http::response<http::string_body> &res);

protected:
    virtual void handle_request(const ReqContext &ctxt, http::response<http::string_body> &res) = 0;

private:
    std::vector<std::shared_ptr<Middleware>> middlewares_;
};
using ApiHandlerPtr = std::shared_ptr<ApiHandler>;