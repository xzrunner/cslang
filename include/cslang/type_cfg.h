#include "cslang/predef.h"

#ifndef TYPE
#error "You must define TYPE macro before include this file"
#endif

TYPE(CHAR,       I1, "char")
TYPE(UCHAR,      U1, "unsigned char")
#ifdef LANG_GLSL
TYPE(BOOL,       I1, "bool")
TYPE(BOOL2,      B,  "bvec2")
TYPE(BOOL3,      B,  "bvec3")
TYPE(BOOL4,      B,  "bvec4")
#endif // LANG_GLSL
TYPE(SHORT,      I2, "short")
TYPE(USHORT,     U2, "unsigned short")
TYPE(INT,        I4, "int")
#ifdef LANG_GLSL
TYPE(INT2,       B,  "ivec2")
TYPE(INT3,       B,  "ivec3")
TYPE(INT4,       B,  "ivec4")
#endif // LANG_GLSL
TYPE(UINT,       U4, "unsigned int")
TYPE(LONG,       I4, "long")
TYPE(ULONG,      U4, "unsigned long")
TYPE(LONGLONG,   I4, "long long")
TYPE(ULONGLONG,  U4, "unsigned long long")
TYPE(ENUM,       I4, "enum")
TYPE(FLOAT,      F4, "float")
#ifdef LANG_GLSL
TYPE(FLOAT2,     B,  "vec2")
TYPE(FLOAT3,     B,  "vec3")
TYPE(FLOAT4,     B,  "vec4")
TYPE(MATRIX2,    B,  "mat2")
TYPE(MATRIX3,    B,  "mat3")
TYPE(MATRIX4,    B,  "mat4")
#endif // LANG_GLSL
TYPE(DOUBLE,     F8, "double")
TYPE(LONGDOUBLE, F8, "long double")
TYPE(POINTER,    U4, "")
TYPE(VOID,       V,  "")
TYPE(UNION,      B,  "")
TYPE(STRUCT,     B,  "")
TYPE(ARRAY,      B,  "")
TYPE(FUNCTION,   B,  "")
