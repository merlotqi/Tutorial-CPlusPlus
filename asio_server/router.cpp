#include "router.h"
#include "req_context.h"
#include <boost/algorithm/string.hpp>

void Router::register_static_handler(const std::string &path, ApiHandlerPtr handler)
{
    get_static_routes().emplace(path, std::move(handler));
}

void Router::register_dynamic_handler(const std::string &path_template, ApiHandlerPtr handler)
{
    std::string regex_str = "^" + std::regex_replace(path_template, std::regex("\\{([^}]+)\\}"), "([^/]+)") + "$";

    std::vector<std::string> param_names;
    std::smatch matches;
    auto it = path_template.cbegin();
    while (std::regex_search(it, path_template.cend(), matches, std::regex("\\{([^}]+)\\}")))
    {
        param_names.push_back(matches[1].str());
        it = matches[0].second;
    }

    get_dynamic_routes().emplace_back(std::make_tuple(std::regex(regex_str), param_names, handler));
}

ApiHandler *Router::route(const std::string &path, ReqContext &ctx)
{
    if (auto it = get_static_routes().find(path); it != get_static_routes().end())
    {
        return it->second.get();
    }

    for (const auto &dynamic_route: get_dynamic_routes())
    {

        const auto &[regex, param_names, handler] = dynamic_route;
        std::smatch matches;
        if (std::regex_match(path, matches, regex))
        {
            std::unordered_map<std::string, std::string> params;
            for (size_t i = 0; i < param_names.size(); ++i)
            {
                params[param_names[i]] = matches[i + 1].str();
            }
            ctx.set_path_params(params);
            return handler.get();
        }
    }

    return nullptr;
}

Router::StaticRoutes &Router::get_static_routes()
{
    static StaticRoutes handlers;
    return handlers;
}

std::vector<Router::DynamicRoute> &Router::get_dynamic_routes()
{
    static std::vector<Router::DynamicRoute> routes;
    return routes;
}
