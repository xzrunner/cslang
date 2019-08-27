#pragma once

namespace vexc
{

enum class VarType
{
    NaN,
    Bool,
    Int,
    Float,
    Double,
};

class Variant
{
public:
    Variant(VarType type);
    explicit Variant(bool b);
    explicit Variant(int  i);
    explicit Variant(float f);
    explicit Variant(double d);

    bool   ToBool() const;
    int    ToInt() const;
    float  ToFloat() const;
    double ToDouble() const;

    VarType type;

    union
    {
        bool   b;
        int    i;
        float  f;
        double d;
    };

}; // Variant

}