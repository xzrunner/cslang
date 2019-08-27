#pragma once

#include "vexc/Expression.h"

namespace vexc
{

void DumpExpression(std::ostream& output,
    const ast::ExprNodePtr& expr, int pos);

}