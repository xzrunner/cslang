#pragma once

#include <set>
#include <string>

namespace cslang
{

class StringPool
{
public:
    const char* InsertAndQuery(const char* str, int len);
    const char* InsertAndQuery(const std::string& str);

    static std::string VoidToString(const void* p);

private:
    std::set<std::string> m_buf;

}; // StringPool

}