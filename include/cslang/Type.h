#pragma once

#include "cslang/predef.h"

#include <string>
#include <memory>

namespace cslang
{

#undef VOID
#undef CONST

enum
{
#define TYPE(type, size, name) type,
#include "cslang/type_cfg.h"
#undef TYPE
};

enum { CONST = 0x1, VOLATILE = 0x2 };

enum { I1, U1, I2, U2, I4, U4, F4, F8, V, B };

struct Type
{
    int categ : 8;
    int qual  : 8;
    int align : 16;
    int size  : 24;

    int b_categ : 8;
};

struct Field
{
    int offset;
    std::string id;
    int bits;
    int pos;
    Type ty;
    std::shared_ptr<Field> next = nullptr;
};

struct RecordType : public Type
{
    std::string id;
    Field flds;
    std::shared_ptr<Field> tail = nullptr;
    int hasConstFld  : 16;
    int hasFlexArray : 16;

}; // RecordType

struct EnumType : public Type
{
    std::string id;

}; // EnumType

struct Signature
{
    int hasProto   : 16;
    int hasEllipse : 16;
//    Vector params;
};

struct FunctionType : public Type
{
    Signature sig;

}; // FunctionType

#define T(categ) (Types[categ])

int TypeCode(const Type& ty);
const Type* Unqual(const Type& ty);
std::unique_ptr<Type> ArrayOf(int len, const Type& ty);

std::string TypeToString(const Type& ty);

void SetupTypeSystem(void);

extern Type WCharType;
extern Type Types[VOID - CHAR + 1];

#define IsIntegType(ty)    (ty->categ <= ENUM)

}