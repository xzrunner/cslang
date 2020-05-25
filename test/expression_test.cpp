#include "utility.h"

#include <cslang/Parser.h>
#include <cslang/Expression.h>
#include <cslang/DumpAST.h>
#include <cslang/EvalAST.h>

#include <catch/catch.hpp>

#include <sstream>
#include <iostream>

TEST_CASE(",")
{
    cslang::Parser parser("1, 2");
    auto expr = cslang::ast::ExpressionParser::ParseExpression(parser);
    auto val = cslang::EvalExpression(expr);
    REQUIRE(val.type == cslang::VarType::Int);
    REQUIRE(val.i == 2);
}

TEST_CASE("=")
{
    SECTION("true")
    {
        cslang::Parser parser("a = 2");
        auto expr = cslang::ast::ExpressionParser::ParseExpression(parser);
        auto val = cslang::EvalExpression(expr);
        REQUIRE(val.type == cslang::VarType::Bool);
        REQUIRE(val.b == true);
    }

    SECTION("false")
    {
        cslang::Parser parser("a = (1 == 2)");
        auto expr = cslang::ast::ExpressionParser::ParseExpression(parser);
        auto val = cslang::EvalExpression(expr);
        REQUIRE(val.type == cslang::VarType::Bool);
        REQUIRE(val.b == false);
    }
}

TEST_CASE("or")
{
    SECTION("true")
    {
        cslang::Parser parser("1 == 1 || 2 == 2");
        auto expr = cslang::ast::ExpressionParser::ParseExpression(parser);
        auto val = cslang::EvalExpression(expr);
        REQUIRE(val.type == cslang::VarType::Bool);
        REQUIRE(val.b == true);
    }
    SECTION("false")
    {
        cslang::Parser parser("1 == 0 || 1 == 2");
        auto expr = cslang::ast::ExpressionParser::ParseExpression(parser);
        auto val = cslang::EvalExpression(expr);
        REQUIRE(val.type == cslang::VarType::Bool);
        REQUIRE(val.b == false);
    }
}

TEST_CASE("and")
{
    SECTION("true")
    {
        cslang::Parser parser("1 == 1 && 2 == 2");
        auto expr = cslang::ast::ExpressionParser::ParseExpression(parser);
        auto val = cslang::EvalExpression(expr);
        REQUIRE(val.type == cslang::VarType::Bool);
        REQUIRE(val.b == true);
    }

    SECTION("false")
    {
        cslang::Parser parser("1 == 1 && 1 == 2");
        auto expr = cslang::ast::ExpressionParser::ParseExpression(parser);
        auto val = cslang::EvalExpression(expr);
        REQUIRE(val.type == cslang::VarType::Bool);
        REQUIRE(val.b == false);
    }
}

TEST_CASE("==")
{
    SECTION("true")
    {
        cslang::Parser parser("1 + 2 == 3");
        auto expr = cslang::ast::ExpressionParser::ParseExpression(parser);
        auto val = cslang::EvalExpression(expr);
        REQUIRE(val.type == cslang::VarType::Bool);
        REQUIRE(val.b == true);
    }

    SECTION("false")
    {
        cslang::Parser parser("1 + 2 == 4");
        auto expr = cslang::ast::ExpressionParser::ParseExpression(parser);
        auto val = cslang::EvalExpression(expr);
        REQUIRE(val.type == cslang::VarType::Bool);
        REQUIRE(val.b == false);
    }
}

TEST_CASE("!=")
{
    SECTION("true")
    {
        cslang::Parser parser("1 + 2 != 4");
        auto expr = cslang::ast::ExpressionParser::ParseExpression(parser);
        auto val = cslang::EvalExpression(expr);
        REQUIRE(val.type == cslang::VarType::Bool);
        REQUIRE(val.b == true);
    }

    SECTION("false")
    {
        cslang::Parser parser("1 + 2 != 3");
        auto expr = cslang::ast::ExpressionParser::ParseExpression(parser);
        auto val = cslang::EvalExpression(expr);
        REQUIRE(val.type == cslang::VarType::Bool);
        REQUIRE(val.b == false);
    }
}

TEST_CASE(">")
{
    SECTION("true")
    {
        cslang::Parser parser("1 + 2 > 2");
        auto expr = cslang::ast::ExpressionParser::ParseExpression(parser);
        auto val = cslang::EvalExpression(expr);
        REQUIRE(val.type == cslang::VarType::Bool);
        REQUIRE(val.b == true);
    }

    SECTION("false")
    {
        cslang::Parser parser("1 + 2 > 4");
        auto expr = cslang::ast::ExpressionParser::ParseExpression(parser);
        auto val = cslang::EvalExpression(expr);
        REQUIRE(val.type == cslang::VarType::Bool);
        REQUIRE(val.b == false);
    }
}

TEST_CASE("<")
{
    SECTION("true")
    {
        cslang::Parser parser("1 + 2 < 4");
        auto expr = cslang::ast::ExpressionParser::ParseExpression(parser);
        auto val = cslang::EvalExpression(expr);
        REQUIRE(val.type == cslang::VarType::Bool);
        REQUIRE(val.b == true);
    }

    SECTION("false")
    {
        cslang::Parser parser("1 + 2 < 2");
        auto expr = cslang::ast::ExpressionParser::ParseExpression(parser);
        auto val = cslang::EvalExpression(expr);
        REQUIRE(val.type == cslang::VarType::Bool);
        REQUIRE(val.b == false);
    }
}

TEST_CASE(">=")
{
    SECTION("true")
    {
        cslang::Parser parser("1 + 2 >= 3");
        auto expr = cslang::ast::ExpressionParser::ParseExpression(parser);
        auto val = cslang::EvalExpression(expr);
        REQUIRE(val.type == cslang::VarType::Bool);
        REQUIRE(val.b == true);
    }

    SECTION("false")
    {
        cslang::Parser parser("1 + 2 >= 4");
        auto expr = cslang::ast::ExpressionParser::ParseExpression(parser);
        auto val = cslang::EvalExpression(expr);
        REQUIRE(val.type == cslang::VarType::Bool);
        REQUIRE(val.b == false);
    }
}

TEST_CASE("<=")
{
    SECTION("true")
    {
        cslang::Parser parser("1 + 2 <= 4");
        auto expr = cslang::ast::ExpressionParser::ParseExpression(parser);
        auto val = cslang::EvalExpression(expr);
        REQUIRE(val.type == cslang::VarType::Bool);
        REQUIRE(val.b == true);
    }

    SECTION("false")
    {
        cslang::Parser parser("1 + 2 <= 2");
        auto expr = cslang::ast::ExpressionParser::ParseExpression(parser);
        auto val = cslang::EvalExpression(expr);
        REQUIRE(val.type == cslang::VarType::Bool);
        REQUIRE(val.b == false);
    }
}

TEST_CASE("+")
{
    SECTION("bool")
    {
        cslang::Parser parser("true + false");
        auto expr = cslang::ast::ExpressionParser::ParseExpression(parser);
        auto val = cslang::EvalExpression(expr);
        REQUIRE(val.type == cslang::VarType::Int);
        REQUIRE(val.i == 1);
    }
    SECTION("int")
    {
        cslang::Parser parser("1 + 2");
        auto expr = cslang::ast::ExpressionParser::ParseExpression(parser);
        auto val = cslang::EvalExpression(expr);
        REQUIRE(val.type == cslang::VarType::Int);
        REQUIRE(val.i == 3);
    }
    SECTION("float")
    {
        cslang::Parser parser("1.0f + 2.0f");
        auto expr = cslang::ast::ExpressionParser::ParseExpression(parser);
        auto val = cslang::EvalExpression(expr);
        REQUIRE(val.type == cslang::VarType::Float);
        REQUIRE(val.f == 3.0f);
    }
    SECTION("double")
    {
        cslang::Parser parser("1.0 + 2.0");
        auto expr = cslang::ast::ExpressionParser::ParseExpression(parser);
        auto val = cslang::EvalExpression(expr);
        REQUIRE(val.type == cslang::VarType::Double);
        REQUIRE(val.d == 3.0);
    }
    SECTION("to int")
    {
        cslang::Parser parser("1 + true");
        auto expr = cslang::ast::ExpressionParser::ParseExpression(parser);
        auto val = cslang::EvalExpression(expr);
        REQUIRE(val.type == cslang::VarType::Int);
        REQUIRE(val.i == 2);
    }
    SECTION("to float")
    {
        cslang::Parser parser("1 + 2.0f");
        auto expr = cslang::ast::ExpressionParser::ParseExpression(parser);
        auto val = cslang::EvalExpression(expr);
        REQUIRE(val.type == cslang::VarType::Float);
        REQUIRE(val.f == 3.0f);
    }
    SECTION("to double")
    {
        cslang::Parser parser("1 + 2.0");
        auto expr = cslang::ast::ExpressionParser::ParseExpression(parser);
        auto val = cslang::EvalExpression(expr);
        REQUIRE(val.type == cslang::VarType::Double);
        REQUIRE(val.d == 3.0);
    }
}

TEST_CASE("-")
{
    cslang::Parser parser("10.0f - 5");
    auto expr = cslang::ast::ExpressionParser::ParseExpression(parser);
    auto val = cslang::EvalExpression(expr);
    REQUIRE(val.type == cslang::VarType::Float);
    REQUIRE(val.f == 5.0f);
}

TEST_CASE("*")
{
    cslang::Parser parser("2 * 3");
    auto expr = cslang::ast::ExpressionParser::ParseExpression(parser);
    auto val = cslang::EvalExpression(expr);
    REQUIRE(val.type == cslang::VarType::Int);
    REQUIRE(val.i == 6);
}

TEST_CASE("/")
{
    SECTION("valid")
    {
        cslang::Parser parser("10 / 2");
        auto expr = cslang::ast::ExpressionParser::ParseExpression(parser);
        auto val = cslang::EvalExpression(expr);
        REQUIRE(val.type == cslang::VarType::Int);
        REQUIRE(val.i == 5);
    }
    SECTION("invalid")
    {
        cslang::Parser parser("10 / 0");
        auto expr = cslang::ast::ExpressionParser::ParseExpression(parser);
        auto val = cslang::EvalExpression(expr);
        REQUIRE(val.type == cslang::VarType::Invalid);
    }
}

TEST_CASE("%")
{
    cslang::Parser parser("11 % 4");
    auto expr = cslang::ast::ExpressionParser::ParseExpression(parser);
    auto val = cslang::EvalExpression(expr);
    REQUIRE(val.type == cslang::VarType::Int);
    REQUIRE(val.i == 3);
}

TEST_CASE("++i")
{
    cslang::Parser parser("++5");
    auto expr = cslang::ast::ExpressionParser::ParseExpression(parser);
    auto val = cslang::EvalExpression(expr);
    REQUIRE(val.type == cslang::VarType::Int);
    REQUIRE(val.i == 6);
}

TEST_CASE("--i")
{
    cslang::Parser parser("--5");
    auto expr = cslang::ast::ExpressionParser::ParseExpression(parser);
    auto val = cslang::EvalExpression(expr);
    REQUIRE(val.type == cslang::VarType::Int);
    REQUIRE(val.i == 4);
}

TEST_CASE("pos")
{
    cslang::Parser parser("+(1 + 2)");
    auto expr = cslang::ast::ExpressionParser::ParseExpression(parser);
    auto val = cslang::EvalExpression(expr);
    REQUIRE(val.type == cslang::VarType::Int);
    REQUIRE(val.i == 3);
}

TEST_CASE("neg")
{
    cslang::Parser parser("-(1 + 2)");
    auto expr = cslang::ast::ExpressionParser::ParseExpression(parser);
    auto val = cslang::EvalExpression(expr);
    REQUIRE(val.type == cslang::VarType::Int);
    REQUIRE(val.i == -3);
}

TEST_CASE("not")
{
    cslang::Parser parser("!(1 > 2)");
    auto expr = cslang::ast::ExpressionParser::ParseExpression(parser);
    auto val = cslang::EvalExpression(expr);
    REQUIRE(val.type == cslang::VarType::Bool);
    REQUIRE(val.b == true);
}

TEST_CASE("complex calc")
{
    SECTION("order")
    {
        cslang::Parser parser("1 + 2 * 3 + 4 - 5");
        auto expr = cslang::ast::ExpressionParser::ParseExpression(parser);
        auto val = cslang::EvalExpression(expr);
        REQUIRE(val.type == cslang::VarType::Int);
        REQUIRE(val.i == 6);
    }

    SECTION("order2")
    {
        cslang::Parser parser("1 + 2 * (3 + 4) - 5");
        auto expr = cslang::ast::ExpressionParser::ParseExpression(parser);
        auto val = cslang::EvalExpression(expr);
        REQUIRE(val.type == cslang::VarType::Int);
        REQUIRE(val.i == 10);
    }

    SECTION("order3")
    {
        cslang::Parser parser("1 + 2 * (3 + 4) - (5 * (2 + 1) - 3)");
        auto expr = cslang::ast::ExpressionParser::ParseExpression(parser);
        auto val = cslang::EvalExpression(expr);
        REQUIRE(val.type == cslang::VarType::Int);
        REQUIRE(val.i == 3);
    }
}

TEST_CASE("ast assign")
{
    auto str = R"(
a = b + c;
)";

    cslang::Parser parser(str);
    auto expr = cslang::ast::ExpressionParser::ParseExpression(parser);

    //std::stringstream ss;
    //cslang::DumpExpression(ss, expr, 0);
    //std::cout << ss.str() << std::endl;
}

TEST_CASE("func floor")
{
    SECTION("assign")
    {
        auto str = R"(
zz = floor(1.1f);
)";

        cslang::Parser parser(str);
        auto expr = cslang::ast::ExpressionParser::ParseExpression(parser);

        //std::stringstream ss;
        //cslang::DumpExpression(ss, expr, 0);
        //std::cout << ss.str() << std::endl;
    }

    SECTION("eval")
    {
        auto str = R"(
floor(1.1f);
)";

        cslang::Parser parser(str);
        auto expr = cslang::ast::ExpressionParser::ParseExpression(parser);
        auto val = cslang::EvalExpression(expr);
        REQUIRE(val.type == cslang::VarType::Float);
        REQUIRE(val.f == 1.0f);
    }
}

TEST_CASE("func max")
{
    auto str = R"(
zz = max(1.1f, 2);
)";

    cslang::Parser parser(str);
    auto expr = cslang::ast::ExpressionParser::ParseExpression(parser);

    //std::stringstream ss;
    //cslang::DumpExpression(ss, expr, 0);
    //std::cout << ss.str() << std::endl;
}

TEST_CASE("attr asign")
{
    auto str = R"(
v@up=v@N;
)";

    cslang::Parser parser(str);
    auto expr = cslang::ast::ExpressionParser::ParseExpression(parser);

    std::stringstream ss;
    cslang::DumpExpression(ss, expr, 0);
    std::cout << ss.str() << std::endl;
}