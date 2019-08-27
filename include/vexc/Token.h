#pragma once

namespace vexc
{

typedef unsigned int TokenType;

enum Token
{
	TK_BEGIN = 0,
#define TOKEN(k, s) k,
#include "vexc/token_cfg.h"
#undef  TOKEN

}; // Token

}