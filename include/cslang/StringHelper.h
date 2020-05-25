#pragma once

#include <string>

namespace cslang
{

class StringHelper
{
public:
    static std::string Format(const std::string str, ...);

}; // StringHelper

}