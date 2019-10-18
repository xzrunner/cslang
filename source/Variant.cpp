#include "vexc/Variant.h"

#include <limits>

#include <assert.h>

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

bool Variant::ToBool() const
{
    switch (type)
    {

    case VarType::Bool:
        return b;
    case VarType::Int:
        return i != 0;
    case VarType::Float:
        return fabs(f) > std::numeric_limits<float>::epsilon();
    case VarType::Double:
        return fabs(d) > std::numeric_limits<double>::epsilon();
    default:
        assert(0);
        return false;
    }
}

int Variant::ToInt() const
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
    default:
        assert(0);
        return 0;
    }
}

float Variant::ToFloat() const
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
    default:
        assert(0);
        return 0;
    }
}

double Variant::ToDouble() const
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
    default:
        assert(0);
        return 0;
    }
}

}