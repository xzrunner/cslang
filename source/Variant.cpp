#include "vexc/Variant.h"

#include "vexc/StringPool.h"
#include "vexc/Parser.h"
#include "vexc/Expression.h"
#include "vexc/EvalAST.h"

#include <limits>

#include <assert.h>

namespace
{

vexc::Variant eval_string_var(const vexc::Variant& var, const void* ud)
{
    if (var.type != vexc::VarType::String) {
        return vexc::Variant();
    }

    std::string str = vexc::StringPool::VoidToString(var.p);
    vexc::Parser parser(str.c_str());
    auto expr = vexc::ast::ExpressionParser::ParseExpression(parser);
    return EvalExpression(expr, ud);
}

}

namespace vexc
{

Variant::Variant(VarType type)
    : type(type)
{
}

Variant::Variant(bool b)
    : type(VarType::Bool)
    , b(b)
{
}

Variant::Variant(int  i)
    : type(VarType::Int)
    , i(i)
{
}

Variant::Variant(float f)
    : type(VarType::Float)
    , f(f)
{
}

Variant::Variant(double d)
    : type(VarType::Double)
    , d(d)
{
}

Variant::Variant(VarType type, const void* p)
    : type(type)
    , p(p)
{
}

bool Variant::ToBool(const void* ud) const
{
    switch (type)
    {
    case VarType::Invalid:
        return false;
    case VarType::Bool:
        return b;
    case VarType::Int:
        return i != 0;
    case VarType::Float:
        return fabs(f) > std::numeric_limits<float>::epsilon();
    case VarType::Float3:
        return true;
    case VarType::Double:
        return fabs(d) > std::numeric_limits<double>::epsilon();
    case VarType::String:
        return eval_string_var(*this, ud).ToBool(ud);
    default:
        assert(0);
        return false;
    }
}

int Variant::ToInt(const void* ud) const
{
    switch (type)
    {
    case VarType::Bool:
        return b ? 1 : 0;
    case VarType::Int:
        return i;
    case VarType::Float:
        return static_cast<int>(f);
    case VarType::Double:
        return static_cast<int>(d);
    case VarType::String:
        return eval_string_var(*this, ud).ToInt(ud);
    default:
        assert(0);
        return 0;
    }
}

float Variant::ToFloat(const void* ud) const
{
    switch (type)
    {
    case VarType::Bool:
        return b ? 1.0f : 0.0f;
    case VarType::Int:
        return static_cast<float>(i);
    case VarType::Float:
        return f;
    case VarType::Double:
        return static_cast<float>(d);
    case VarType::String:
        return eval_string_var(*this, ud).ToFloat(ud);
    default:
        assert(0);
        return 0;
    }
}

double Variant::ToDouble(const void* ud) const
{
    switch (type)
    {
    case VarType::Bool:
        return b ? 1 : 0;
    case VarType::Int:
        return i;
    case VarType::Float:
        return f;
    case VarType::Double:
        return d;
    case VarType::String:
        return eval_string_var(*this, ud).ToDouble(ud);
    default:
        assert(0);
        return 0;
    }
}

}