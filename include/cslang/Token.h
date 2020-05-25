#pragma once

namespace cslang
{

typedef unsigned int TokenType;

enum Token
{
	TK_BEGIN = 0,
#define TOKEN(k, s) k,
#include "cslang/token_cfg.h"
#undef  TOKEN

}; // Token

}