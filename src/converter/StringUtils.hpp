#pragma once

#include <vector>
#include <string>
#include <string_view>
#include <sstream>

namespace StringUtils
{

template<typename T>
std::vector<std::basic_string<T>> Split(std::basic_string_view<T> view, T delim)
{
    std::vector<std::basic_string<T>> result;

    size_t startIndex = 0;
    size_t endIndex = 0;
    while ((endIndex = view.find(delim, startIndex)) < view.size())
    {
        std::string s{ view.substr(startIndex, endIndex - startIndex) };
        result.push_back(s);
        startIndex = endIndex + 1;
    }

    if (startIndex < view.size())
    {
        std::string s{ view.substr(startIndex) };
        result.push_back(s);
    }

    return result;
}

template<typename T>
std::vector<T> Split(const T* str, const T& delim)
{
    return Split(std::basic_string_view<typename T::value_type>(str), delim);
}

template<typename T>
std::vector<std::basic_string<T>> Split(std::basic_string<T> str, const std::basic_string<T>& delim)
{
    std::vector<std::basic_string<T>> result;
    size_t pos = 0;
    std::basic_string<T> token;
    while ((pos = str.find(delim)) != std::basic_string<T>::npos)
    {
        token = str.substr(0, pos);
        result.push_back(token);
        str.erase(0, pos + delim.length());
    }
    result.push_back(str);

    return result;
}

template<typename T>
std::vector<std::basic_string<T>> Split(const std::basic_string<T>& str, T delim)
{
    std::vector<std::basic_string<T>> result;

    std::basic_stringstream<T> sstr(str);
    std::basic_string<T> buf;
    while (std::getline(sstr, buf, delim))
    {
        result.push_back(buf);
    }

    return result;
}

template<typename string_t>
string_t ltrim(string_t str)
{
    auto it = std::find_if(str.begin(), str.end(),
    [](typename string_t::value_type c) {
        return !std::isspace<string_t::value_type>(c, std::locale::classic());
    });
    str.erase(str.begin(), it);
    return str;
}

template<typename string_t>
string_t rtrim(string_t str)
{
    auto it = std::find_if(str.rbegin(), str.rend(),
        [](typename string_t::value_type c)
        {
            return !std::isspace<string_t::value_type>(c, std::locale::classic());
        });
    str.erase(it.base(), str.end());
    return str;
}

template<typename string_t>
string_t Trim(string_t str)
{
    return ltrim(rtrim(str));
}

}