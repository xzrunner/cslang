#pragma once

#include "cslang/Expression.h"

namespace cslang
{

void DumpExpression(std::ostream& output,
    const ast::ExprNodePtr& expr, int pos);

}