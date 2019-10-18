#include "utility.h"

#include <vexc/Parser.h>
#include <vexc/Expression.h>
#include <vexc/DumpAST.h>
#include <vexc/EvalAST.h>

#include <catch/catch.hpp>

#include <sstream>
#include <iostream>

TEST_CASE(",")
{
    vexc::Parser parser("1, 2");
    auto expr = vexc::ast::ExpressionParser::ParseExpression(parser);
    auto val = vexc::EvalExpression(expr);
    REQUIRE(val.type == vexc::VarType::Int);
    REQUIRE(val.i == 2);
}

TEST_CASE("=")
{
    SECTION("true")
    {
        vexc::Parser parser("a = 2");
        auto expr = vexc::ast::ExpressionParser::ParseExpression(parser);
        auto val = vexc::EvalExpression(expr);
        REQUIRE(val.type == vexc::VarType::Bool);
        REQUIRE(val.b == true);
    }

    SECTION("false")
    {
        vexc::Parser parser("a = (1 == 2)");
        auto expr = vexc::ast::ExpressionParser::ParseExpression(parser);
        auto val = vexc::EvalExpression(expr);
        REQUIRE(val.type == vexc::VarType::Bool);
        REQUIRE(val.b == false);
    }
}

TEST_CASE("or")
{
    SECTION("true")
    {
        vexc::Parser parser("1 == 1 || 2 == 2");
        auto expr = vexc::ast::ExpressionParser::ParseExpression(parser);
        auto val = vexc::EvalExpression(expr);
        REQUIRE(val.type == vexc::VarType::Bool);
        REQUIRE(val.b == true);
    }
    SECTION("false")
    {
        vexc::Parser parser("1 == 0 || 1 == 2");
        auto expr = vexc::ast::ExpressionParser::ParseExpression(parser);
        auto val = vexc::EvalExpression(expr);
        REQUIRE(val.type == vexc::VarType::Bool);
        REQUIRE(val.b == false);
    }
}

TEST_CASE("and")
{
    SECTION("true")
    {
        vexc::Parser parser("1 == 1 && 2 == 2");
        auto expr = vexc::ast::ExpressionParser::ParseExpression(parser);
        auto val = vexc::EvalExpression(expr);
        REQUIRE(val.type == vexc::VarType::Bool);
        REQUIRE(val.b == true);
    }

    SECTION("false")
    {
        vexc::Parser parser("1 == 1 && 1 == 2");
        auto expr = vexc::ast::ExpressionParser::ParseExpression(parser);
        auto val = vexc::EvalExpression(expr);
        REQUIRE(val.type == vexc::VarType::Bool);
        REQUIRE(val.b == false);
    }
}

TEST_CASE("==")
{
    SECTION("true")
    {
        vexc::Parser parser("1 + 2 == 3");
        auto expr = vexc::ast::ExpressionParser::ParseExpression(parser);
        auto val = vexc::EvalExpression(expr);
        REQUIRE(val.type == vexc::VarType::Bool);
        REQUIRE(val.b == true);
    }

    SECTION("false")
    {
        vexc::Parser parser("1 + 2 == 4");
        auto expr = vexc::ast::ExpressionParser::ParseExpression(parser);
        auto val = vexc::EvalExpression(expr);
        REQUIRE(val.type == vexc::VarType::Bool);
        REQUIRE(val.b == false);
    }
}

TEST_CASE("!=")
{
    SECTION("true")
    {
        vexc::Parser parser("1 + 2 != 4");
        auto expr = vexc::ast::ExpressionParser::ParseExpression(parser);
        auto val = vexc::EvalExpression(expr);
        REQUIRE(val.type == vexc::VarType::Bool);
        REQUIRE(val.b == true);
    }

    SECTION("false")
    {
        vexc::Parser parser("1 + 2 != 3");
        auto expr = vexc::ast::ExpressionParser::ParseExpression(parser);
        auto val = vexc::EvalExpression(expr);
        REQUIRE(val.type == vexc::VarType::Bool);
        REQUIRE(val.b == false);
    }
}

TEST_CASE(">")
{
    SECTION("true")
    {
        vexc::Parser parser("1 + 2 > 2");
        auto expr = vexc::ast::ExpressionParser::ParseExpression(parser);
        auto val = vexc::EvalExpression(expr);
        REQUIRE(val.type == vexc::VarType::Bool);
        REQUIRE(val.b == true);
    }

    SECTION("false")
    {
        vexc::Parser parser("1 + 2 > 4");
        auto expr = vexc::ast::ExpressionParser::ParseExpression(parser);
        auto val = vexc::EvalExpression(expr);
        REQUIRE(val.type == vexc::VarType::Bool);
        REQUIRE(val.b == false);
    }
}

TEST_CASE("<")
{
    SECTION("true")
    {
        vexc::Parser parser("1 + 2 < 4");
        auto expr = vexc::ast::ExpressionParser::ParseExpression(parser);
        auto val = vexc::EvalExpression(expr);
        REQUIRE(val.type == vexc::VarType::Bool);
        REQUIRE(val.b == true);
    }

    SECTION("false")
    {
        vexc::Parser parser("1 + 2 < 2");
        auto expr = vexc::ast::ExpressionParser::ParseExpression(parser);
        auto val = vexc::EvalExpression(expr);
        REQUIRE(val.type == vexc::VarType::Bool);
        REQUIRE(val.b == false);
    }
}

TEST_CASE(">=")
{
    SECTION("true")
    {
        vexc::Parser parser("1 + 2 >= 3");
        auto expr = vexc::ast::ExpressionParser::ParseExpression(parser);
        auto val = vexc::EvalExpression(expr);
        REQUIRE(val.type == vexc::VarType::Bool);
        REQUIRE(val.b == true);
    }

    SECTION("false")
    {
        vexc::Parser parser("1 + 2 >= 4");
        auto expr = vexc::ast::ExpressionParser::ParseExpression(parser);
        auto val = vexc::EvalExpression(expr);
        REQUIRE(val.type == vexc::VarType::Bool);
        REQUIRE(val.b == false);
    }
}

TEST_CASE("<=")
{
    SECTION("true")
    {
        vexc::Parser parser("1 + 2 <= 4");
        auto expr = vexc::ast::ExpressionParser::ParseExpression(parser);
        auto val = vexc::EvalExpression(expr);
        REQUIRE(val.type == vexc::VarType::Bool);
        REQUIRE(val.b == true);
    }

    SECTION("false")
    {
        vexc::Parser parser("1 + 2 <= 2");
        auto expr = vexc::ast::ExpressionParser::ParseExpression(parser);
        auto val = vexc::EvalExpression(expr);
        REQUIRE(val.type == vexc::VarType::Bool);
        REQUIRE(val.b == false);
    }
}

TEST_CASE("+")
{
    SECTION("bool")
    {
        vexc::Parser parser("true + false");
        auto expr = vexc::ast::ExpressionParser::ParseExpression(parser);
        auto val = vexc::EvalExpression(expr);
        REQUIRE(val.type == vexc::VarType::Int);
        REQUIRE(val.i == 1);
    }
    SECTION("int")
    {
        vexc::Parser parser("1 + 2");
        auto expr = vexc::ast::ExpressionParser::ParseExpression(parser);
        auto val = vexc::EvalExpression(expr);
        REQUIRE(val.type == vexc::VarType::Int);
        REQUIRE(val.i == 3);
    }
    SECTION("float")
    {
        vexc::Parser parser("1.0f + 2.0f");
        auto expr = vexc::ast::ExpressionParser::ParseExpression(parser);
        auto val = vexc::EvalExpression(expr);
        REQUIRE(val.type == vexc::VarType::Float);
        REQUIRE(val.f == 3.0f);
    }
    SECTION("double")
    {
        vexc::Parser parser("1.0 + 2.0");
        auto expr = vexc::ast::ExpressionParser::ParseExpression(parser);
        auto val = vexc::EvalExpression(expr);
        REQUIRE(val.type == vexc::VarType::Double);
        REQUIRE(val.d == 3.0);
    }
    SECTION("to int")
    {
        vexc::Parser parser("1 + true");
        auto expr = vexc::ast::ExpressionParser::ParseExpression(parser);
        auto val = vexc::EvalExpression(expr);
        REQUIRE(val.type == vexc::VarType::Int);
        REQUIRE(val.i == 2);
    }
    SECTION("to float")
    {
        vexc::Parser parser("1 + 2.0f");
        auto expr = vexc::ast::ExpressionParser::ParseExpression(parser);
        auto val = vexc::EvalExpression(expr);
        REQUIRE(val.type == vexc::VarType::Float);
        REQUIRE(val.f == 3.0f);
    }
    SECTION("to double")
    {
        vexc::Parser parser("1 + 2.0");
        auto expr = vexc::ast::ExpressionParser::ParseExpression(parser);
        auto val = vexc::EvalExpression(expr);
        REQUIRE(val.type == vexc::VarType::Double);
        REQUIRE(val.d == 3.0);
    }
}

TEST_CASE("-")
{
    vexc::Parser parser("10.0f - 5");
    auto expr = vexc::ast::ExpressionParser::ParseExpression(parser);
    auto val = vexc::EvalExpression(expr);
    REQUIRE(val.type == vexc::VarType::Float);
    REQUIRE(val.f == 5.0f);
}

TEST_CASE("*")
{
    vexc::Parser parser("2 * 3");
    auto expr = vexc::ast::ExpressionParser::ParseExpression(parser);
    auto val = vexc::EvalExpression(expr);
    REQUIRE(val.type == vexc::VarType::Int);
    REQUIRE(val.i == 6);
}

TEST_CASE("/")
{
    SECTION("valid")
    {
        vexc::Parser parser("10 / 2");
        auto expr = vexc::ast::ExpressionParser::ParseExpression(parser);
        auto val = vexc::EvalExpression(expr);
        REQUIRE(val.type == vexc::VarType::Int);
        REQUIRE(val.i == 5);
    }
    SECTION("invalid")
    {
        vexc::Parser parser("10 / 0");
        auto expr = vexc::ast::ExpressionParser::ParseExpression(parser);
        auto val = vexc::EvalExpression(expr);
        REQUIRE(val.type == vexc::VarType::Invalid);
    }
}

TEST_CASE("%")
{
    vexc::Parser parser("11 % 4");
    auto expr = vexc::ast::ExpressionParser::ParseExpression(parser);
    auto val = vexc::EvalExpression(expr);
    REQUIRE(val.type == vexc::VarType::Int);
    REQUIRE(val.i == 3);
}

TEST_CASE("++i")
{
    vexc::Parser parser("++5");
    auto expr = vexc::ast::ExpressionParser::ParseExpression(parser);
    auto val = vexc::EvalExpression(expr);
    REQUIRE(val.type == vexc::VarType::Int);
    REQUIRE(val.i == 6);
}

TEST_CASE("--i")
{
    vexc::Parser parser("--5");
    auto expr = vexc::ast::ExpressionParser::ParseExpression(parser);
    auto val = vexc::EvalExpression(expr);
    REQUIRE(val.type == vexc::VarType::Int);
    REQUIRE(val.i == 4);
}

TEST_CASE("pos")
{
    vexc::Parser parser("+(1 + 2)");
    auto expr = vexc::ast::ExpressionParser::ParseExpression(parser);
    auto val = vexc::EvalExpression(expr);
    REQUIRE(val.type == vexc::VarType::Int);
    REQUIRE(val.i == 3);
}

TEST_CASE("neg")
{
    vexc::Parser parser("-(1 + 2)");
    auto expr = vexc::ast::ExpressionParser::ParseExpression(parser);
    auto val = vexc::EvalExpression(expr);
    REQUIRE(val.type == vexc::VarType::Int);
    REQUIRE(val.i == -3);
}

TEST_CASE("not")
{
    vexc::Parser parser("!(1 > 2)");
    auto expr = vexc::ast::ExpressionParser::ParseExpression(parser);
    auto val = vexc::EvalExpression(expr);
    REQUIRE(val.type == vexc::VarType::Bool);
    REQUIRE(val.b == true);
}

TEST_CASE("complex calc")
{
    SECTION("order")
    {
        vexc::Parser parser("1 + 2 * 3 + 4 - 5");
        auto expr = vexc::ast::ExpressionParser::ParseExpression(parser);
        auto val = vexc::EvalExpression(expr);
        REQUIRE(val.type == vexc::VarType::Int);
        REQUIRE(val.i == 6);
    }

    SECTION("order2")
    {
        vexc::Parser parser("1 + 2 * (3 + 4) - 5");
        auto expr = vexc::ast::ExpressionParser::ParseExpression(parser);
        auto val = vexc::EvalExpression(expr);
        REQUIRE(val.type == vexc::VarType::Int);
        REQUIRE(val.i == 10);
    }

    SECTION("order3")
    {
        vexc::Parser parser("1 + 2 * (3 + 4) - (5 * (2 + 1) - 3)");
        auto expr = vexc::ast::ExpressionParser::ParseExpression(parser);
        auto val = vexc::EvalExpression(expr);
        REQUIRE(val.type == vexc::VarType::Int);
        REQUIRE(val.i == 3);
    }
}

TEST_CASE("ast assign")
{
    auto str = R"(
a = b + c;
)";

    vexc::Parser parser(str);
    auto expr = vexc::ast::ExpressionParser::ParseExpression(parser);

    //std::stringstream ss;
    //vexc::DumpExpression(ss, expr, 0);
    //std::cout << ss.str() << std::endl;
}

TEST_CASE("func floor")
{
    SECTION("assign")
    {
        auto str = R"(
zz = floor(1.1f);
)";

        vexc::Parser parser(str);
        auto expr = vexc::ast::ExpressionParser::ParseExpression(parser);

        //std::stringstream ss;
        //vexc::DumpExpression(ss, expr, 0);
        //std::cout << ss.str() << std::endl;
    }

    SECTION("eval")
    {
        auto str = R"(
floor(1.1f);
)";

        vexc::Parser parser(str);
        auto expr = vexc::ast::ExpressionParser::ParseExpression(parser);
        auto val = vexc::EvalExpression(expr);
        REQUIRE(val.type == vexc::VarType::Float);
        REQUIRE(val.f == 1.0f);
    }
}

TEST_CASE("func max")
{
    auto str = R"(
zz = max(1.1f, 2);
)";

    vexc::Parser parser(str);
    auto expr = vexc::ast::ExpressionParser::ParseExpression(parser);

    //std::stringstream ss;
    //vexc::DumpExpression(ss, expr, 0);
    //std::cout << ss.str() << std::endl;
}

TEST_CASE("attr asign")
{
    auto str = R"(
v@up=v@N;
)";

    vexc::Parser parser(str);
    auto expr = vexc::ast::ExpressionParser::ParseExpression(parser);

    std::stringstream ss;
    vexc::DumpExpression(ss, expr, 0);
    std::cout << ss.str() << std::endl;
}