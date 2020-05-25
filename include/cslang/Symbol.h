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

}