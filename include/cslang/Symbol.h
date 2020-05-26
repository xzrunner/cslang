#pragma once

#include "cslang/Type.h"
#include "cslang/Tokenizer.h"

namespace cslang
{

struct Symbol
{
    int kind;
    char *name;
    char *aname;
    Type ty;
    int level;
    int sclass;
    int ref;
    int defined   : 1;
    int addressed : 1;
    int needwb    : 1;
    int unused    : 29;
    Value   val;
    Symbol* reg;
    Symbol* link;
    Symbol* next;

}; // Symbol

struct ValueDef
{
    Symbol dst;
    int op;
    Symbol src1;
    Symbol src2;
    //BBlock ownBB;
    struct valueDef *link;
};

struct ValueUse
{
    ValueDef def;
    struct valueUse *next;
};

struct VariableSymbol : public Symbol
{
    InitData idata;
    ValueDef def;
    ValueUse uses;
    int offset;

}; // VariableSymbol

struct FunctionSymbol : public Symbol
{
	Symbol params;
	Symbol locals;
	Symbol *lastv;
	int nbblock;
	//BBlock entryBB;
	//BBlock exitBB;
	ValueDef valNumTable[16];

}; // FunctionSymbol

}