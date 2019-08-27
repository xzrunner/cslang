#pragma once

#include <set>
#include <string>

namespace vexc
{

class StringPool
{
public:
    const char* InsertAndQuery(const char* str, int len);
    const char* InsertAndQuery(const std::string& str);

private:
    std::set<std::string> m_buf;

}; // StringPool

}