#include "utility.h"

#include <cslang/Parser.h>
#include <cslang/Expression.h>
#include <cslang/DumpAST.h>
#include <cslang/EvalAST.h>

#include <catch2/catch_test_macros.hpp>

#include <sstream>
#include <iostream>

TEST_CASE("ch")
{
    auto str = R"(
ch("../../House_Width")
)";

    cslang::Parser parser(str);
    auto expr = cslang::ast::ExpressionParser::ParseExpression(parser);

    //std::stringstream ss;
    //cslang::DumpExpression(ss, expr, 0);
    //std::cout << ss.str() << std::endl;
}

TEST_CASE("getbbox_min")
{
    auto str = R"(
getbbox_min(0).y
)";

    cslang::Parser parser(str);
    auto expr = cslang::ast::ExpressionParser::ParseExpression(parser);

    //std::stringstream ss;
    //cslang::DumpExpression(ss, expr, 0);
    //std::cout << ss.str() << std::endl;
}

TEST_CASE("attr")
{
    auto str = R"(
@P.y == getbbox_min(0).y
)";

    cslang::Parser parser(str);
    auto expr = cslang::ast::ExpressionParser::ParseExpression(parser);

    //std::stringstream ss;
    //cslang::DumpExpression(ss, expr, 0);
    //std::cout << ss.str() << std::endl;
}

TEST_CASE("expr")
{
    auto str = R"(
$SIZEX == $SIZEY
)";

    cslang::Parser parser(str);
    auto expr = cslang::ast::ExpressionParser::ParseExpression(parser);

    //std::stringstream ss;
    //cslang::DumpExpression(ss, expr, 0);
    //std::cout << ss.str() << std::endl;
}