#pragma once

#include <router.h>
#include <session.h>

class version_handler : public ApiHandler
{
public:
    void handle_request(const ReqContext &ctx, http::response<http::string_body> &res) override;
};
REGISTER_STATIC_HANDLER("/version", version_handler)