#include "api_handler.h"
#include "middleware.h"
#include <regex>

class functional_middleware : public Middleware
{
    std::function<void(const ReqContext &ctxt, http::response<http::string_body> &res, std::function<void()> next)>
            func_;

public:
    functional_middleware(std::function<void(const ReqContext &ctxt, http::response<http::string_body> &res,
                                             std::function<void()> next)>
                                  func)
        : func_(func)
    {
    }

    void operator()(const ReqContext &ctxt, http::response<http::string_body> &res, std::function<void()> next) override
    {
        func_(ctxt, res, next);
    }
};

void ApiHandler::use(std::shared_ptr<Middleware> middleware)
{
    middlewares_.push_back(middleware);
}
void ApiHandler::use(
        std::function<void(const ReqContext &ctxt, http::response<http::string_body> &res, std::function<void()> next)>
                middleware_func)
{
    middlewares_.push_back(std::make_shared<functional_middleware>(middleware_func));
}

void ApiHandler::execute(const ReqContext &ctxt, http::response<http::string_body> &res)
{
    size_t index = 0;
    std::function<void()> next = [&, this]() {
        if (index < middlewares_.size())
        {
            auto &&middleware = middlewares_[index++];
            (*middleware)(ctxt, res, next);
        }
        else
        {
            handle_request(ctxt, res);
        }
    };

    next();// start the middleware chain
}
