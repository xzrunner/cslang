#pragma once

#include "vexc/Variant.h"
#include "vexc/Expression.h"

namespace vexc
{

Variant EvalExpression(const ast::ExprNodePtr& expr);

}