#pragma once

#include "api_handler.h"
#include <boost/beast/http.hpp>
#include <map>
#include <memory>
#include <regex>
#include <vector>

namespace http = boost::beast::http;

class ReqContext;
class Router
{
public:
    static void register_static_handler(const std::string &path, ApiHandlerPtr handler);
    static void register_dynamic_handler(const std::string &path, ApiHandlerPtr handler);
    static ApiHandler *route(const std::string &path, ReqContext &ctx);

private:
    using StaticRoutes = std::map<std::string, ApiHandlerPtr>;
    static StaticRoutes &get_static_routes();

    using DynamicRoute = std::tuple<std::regex, std::vector<std::string>, ApiHandlerPtr>;

    static std::vector<DynamicRoute> &get_dynamic_routes();
};

#define REGISTER_STATIC_HANDLER(Path, HandlerClass)                                  \
    namespace {                                                                      \
    struct HandlerClass##Register                                                    \
    {                                                                                \
        HandlerClass##Register()                                                     \
        {                                                                            \
            Router::register_static_handler(Path, std::make_shared<HandlerClass>()); \
        }                                                                            \
    } HandlerClass##_register;                                                       \
    }

#define REGISTER_DYNAMIC_HANDLER(Path, HandlerClass)                                  \
    namespace {                                                                       \
    struct HandlerClass##Register                                                     \
    {                                                                                 \
        HandlerClass##Register()                                                      \
        {                                                                             \
            Router::register_dynamic_handler(Path, std::make_shared<HandlerClass>()); \
        }                                                                             \
    } HandlerClass##_register;                                                        \
    }