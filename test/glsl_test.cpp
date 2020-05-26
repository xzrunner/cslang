#include "utility.h"

#include <cslang/Parser.h>
#include <cslang/Statement.h>
#include <cslang/DumpAST.h>

#include <catch/catch.hpp>

#include <sstream>
#include <iostream>

TEST_CASE("ellipse")
{
    auto str = R"(

float ellipse(vec2 uv, float width, float height)
{
    float d = length((uv * 2 - 1) / vec2(width, height));
    return clamp((1 - d) / fwidth(d), 0.0, 1.0);
}

)";

    cslang::Parser parser(str);
    auto stat = cslang::ast::DeclarationParser::ParseTranslationUnit(parser);

    std::stringstream ss;
    cslang::DumpTranslationUnit(ss, stat);
    std::cout << ss.str() << std::endl;
}
