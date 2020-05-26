#pragma once

#include "cslang/predef.h"

#define FIRST_DECLARATION_BASE                                      \
    TK_AUTO,   TK_EXTERN,   TK_REGISTER, TK_STATIC,   TK_TYPEDEF,   \
    TK_CONST,  TK_VOLATILE, TK_SIGNED,   TK_UNSIGNED, TK_SHORT,     \
    TK_LONG,   TK_CHAR,     TK_INT,      TK_INT64,    TK_FLOAT,     \
    TK_DOUBLE, TK_ENUM,     TK_STRUCT,   TK_UNION,    TK_VOID, TK_ID

#ifdef LANG_GLSL
#define FIRST_DECLARATION FIRST_DECLARATION_BASE,                       \
    TK_BOOL, TK_BOOL2, TK_BOOL3, TK_BOOL4, TK_INT2, TK_INT3, TK_INT4,   \
    TK_FLOAT2, TK_FLOAT3, TK_FLOAT4, TK_MATRIX2, TK_MATRIX3, TK_MATRIX4
#else
#define FIRST_DECLARATION FIRST_DECLARATION_BASE
#endif // LANG_GLSL

#define FIRST_EXPRESSION                                                          \
    TK_SIZEOF,       TK_ID,         TK_INTCONST,    TK_UINTCONST,  TK_LONGCONST,  \
    TK_ULONGCONST,   TK_LLONGCONST, TK_ULLONGCONST, TK_FLOATCONST, TK_DOUBLECONST,\
    TK_LDOUBLECONST, TK_STRING,     TK_WIDESTRING,  TK_BITAND,     TK_ADD,        \
    TK_SUB,          TK_MUL,        TK_INC,         TK_DEC,        TK_NOT,        \
    TK_COMP,         TK_LPAREN

#define FIRST_STATEMENT                                                                   \
    TK_BREAK, TK_CASE,   TK_CONTINUE, TK_DEFAULT, TK_DO,    TK_FOR,       TK_GOTO,        \
    TK_IF,    TK_LBRACE, TK_RETURN,   TK_SWITCH,  TK_WHILE, TK_SEMICOLON, FIRST_EXPRESSION
