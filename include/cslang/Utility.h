#pragma once

#include <string>

namespace cslang
{

std::string StringFormat(const std::string str, ...);

int TokenTypeToSymbolType(unsigned int token);

std::string PrintFloatConstant(float f);
std::string PrintDoubleConstant(double d);

}