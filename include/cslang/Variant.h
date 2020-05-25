#pragma once

namespace cslang
{

enum class VarType
{
    Invalid = 0,

    Bool,
    Int,
    Float,
    Float3,
    Double,
    String,
};

class Variant
{
public:
    Variant() : type(VarType::Invalid) {}
    Variant(VarType type);
    explicit Variant(bool b);
    explicit Variant(int  i);
    explicit Variant(float f);
    explicit Variant(double d);
    explicit Variant(VarType type, const void* p);

    bool   ToBool(const void* ud) const;
    int    ToInt(const void* ud) const;
    float  ToFloat(const void* ud) const;
    double ToDouble(const void* ud) const;

    VarType type = VarType::Invalid;

    union
    {
        bool   b;
        int    i;
        float  f;
        double d;
        const void* p;
    };

}; // Variant

}