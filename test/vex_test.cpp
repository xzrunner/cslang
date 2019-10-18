#include "utility.h"

#include <vexc/Parser.h>
#include <vexc/Expression.h>
#include <vexc/DumpAST.h>
#include <vexc/EvalAST.h>

#include <catch/catch.hpp>

#include <sstream>
#include <iostream>

TEST_CASE("ch")
{
    auto str = R"(
ch("../../House_Width")
)";

    vexc::Parser parser(str);
    auto expr = vexc::ast::ExpressionParser::ParseExpression(parser);

    //std::stringstream ss;
    //vexc::DumpExpression(ss, expr, 0);
    //std::cout << ss.str() << std::endl;
}

TEST_CASE("getbbox_min")
{
    auto str = R"(
getbbox_min(0).y
)";

    vexc::Parser parser(str);
    auto expr = vexc::ast::ExpressionParser::ParseExpression(parser);

    //std::stringstream ss;
    //vexc::DumpExpression(ss, expr, 0);
    //std::cout << ss.str() << std::endl;
}

TEST_CASE("attr")
{
    auto str = R"(
@P.y == getbbox_min(0).y
)";

    vexc::Parser parser(str);
    auto expr = vexc::ast::ExpressionParser::ParseExpression(parser);

    //std::stringstream ss;
    //vexc::DumpExpression(ss, expr, 0);
    //std::cout << ss.str() << std::endl;
}

TEST_CASE("expr")
{
    auto str = R"(
$SIZEX == $SIZEY
)";

    vexc::Parser parser(str);
    auto expr = vexc::ast::ExpressionParser::ParseExpression(parser);

    //std::stringstream ss;
    //vexc::DumpExpression(ss, expr, 0);
    //std::cout << ss.str() << std::endl;
}