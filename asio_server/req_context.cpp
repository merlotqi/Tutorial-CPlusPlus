#include "req_context.h"
#include <regex>

static std::string url_decode(const std::string &str)
{
    std::string ret;
    ret.reserve(str.size());

    for (size_t i = 0; i < str.size(); ++i)
    {
        if (str[i] == '%')
        {
            if (i + 2 < str.size())
            {
                try
                {
                    int value = std::stoi(str.substr(i + 1, 2), nullptr, 16);
                    ret += static_cast<char>(value);
                    i += 2;
                }
                catch (const std::exception &)
                {
                    ret += '%';
                }
            }
            else
            {
                ret += '%';
            }
        }
        else if (str[i] == '+')
        {
            ret += ' ';
        }
        else
        {
            ret += str[i];
        }
    }

    return ret;
}

void ReqContext::parse_query_(const std::string &query)
{
    query_params_.clear();
    std::istringstream iss(query);
    std::string pair;

    std::regex re_index(R"((\w+)(\[\d*\]|\[\])?)");

    while (std::getline(iss, pair, '&'))
    {
        if (pair.empty())
            continue;

        auto pos = pair.find('=');
        std::string key, value;
        if (pos != std::string::npos)
        {
            key = pair.substr(0, pos);
            value = pair.substr(pos + 1);
        }
        else
        {
            key = pair;
            value = "";
        }

        key = url_decode(key);
        value = url_decode(value);

        std::smatch m;
        if (std::regex_match(key, m, re_index))
        {
            key = m[1];
        }

        std::istringstream vs(value);
        std::string v;
        while (std::getline(vs, v, ','))
        {
            if (!v.empty())
                query_params_[key].push_back(v);
        }
    }
}


ReqContext::ReqContext(const http::request<http::string_body> &req_) : req_(req_)
{
    parse_query_(query_path());
}

void ReqContext::set_path_params(const std::unordered_map<std::string, std::string> &params)
{
    path_params_ = params;
}

const http::request<http::string_body> &ReqContext::raw_req() const
{
    return req_;
}

http::verb ReqContext::method() const
{
    return req_.method();
}

std::string ReqContext::method_string() const
{
    return req_.method_string();
}


std::string ReqContext::header(const std::string &key) const
{
    auto it = req_.find(key);
    if (it != req_.end())
        return it->value();
    return {};
}

std::string ReqContext::header(http::field key) const
{
    auto it = req_.find(key);
    if (it != req_.end())
        return it->value();
    return {};
}

std::string ReqContext::body() const
{
    return req_.body();
}

std::string ReqContext::query_path() const
{
    std::string target = req_.target();
    auto pos = target.find('?');
    if (pos != std::string::npos)
        return target.substr(pos + 1);
    return {};
}

std::unordered_map<std::string, std::vector<std::string>> ReqContext::query_params() const
{
    return query_params_;
}

std::string ReqContext::path_param(const std::string &key) const
{
    if (has_path_param(key))
        return path_params().at(key);
    return {};
}

bool ReqContext::has_path_param(const std::string &key) const
{
    return path_params().find(key) != path_params().end();
}

std::unordered_map<std::string, std::string> ReqContext::path_params() const
{
    return path_params_;
}