#pragma once

#include <boost/beast/http.hpp>

namespace http = boost::beast::http;

class ReqContext
{
    http::request<http::string_body> req_;
    std::unordered_map<std::string, std::string> path_params_;
    std::unordered_map<std::string, std::vector<std::string>> query_params_;

public:
    ReqContext(const http::request<http::string_body> &req);
    void set_path_params(const std::unordered_map<std::string, std::string> &params);

    const http::request<http::string_body> &raw_req() const;

    http::verb method() const;
    std::string method_string() const;

    std::string header(const std::string &key) const;
    std::string header(http::field key) const;
    std::string body() const;
    std::string query_path() const;
    std::unordered_map<std::string, std::vector<std::string>> query_params() const;

    std::string path_param(const std::string &key) const;
    bool has_path_param(const std::string &key) const;
    std::unordered_map<std::string, std::string> path_params() const;

private:
    void parse_query_(const std::string &query);
};