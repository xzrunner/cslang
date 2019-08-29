#include "vexc/StringPool.h"

namespace vexc
{

const char* StringPool::InsertAndQuery(const char* str, int len)
{
    return InsertAndQuery(std::string(str, len));
}

const char* StringPool::InsertAndQuery(const std::string& str)
{
    auto itr = m_buf.find(str);
    if (itr == m_buf.end()) {
        auto itr = m_buf.insert(str);
        return itr.first->data();
    } else {
        return itr->data();
    }
}

std::string StringPool::VoidToString(const void* p)
{
    return std::string(static_cast<const char*>(p));
}

}