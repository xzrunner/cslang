#pragma once

#include "vexc/Variant.h"
#include "vexc/Expression.h"

#include <vector>

namespace vexc
{

Variant EvalExpression(const ast::ExprNodePtr& expr);

void RegistBuildInFunc(const std::string& name, std::function<Variant(const std::vector<Variant>&)> func);

}