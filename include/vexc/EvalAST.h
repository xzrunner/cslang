#pragma once

#include "vexc/Variant.h"
#include "vexc/Expression.h"

#include <vector>

namespace vexc
{

Variant EvalExpression(const ast::ExprNodePtr& expr);

using BuildInFunc = std::function<Variant(const std::vector<Variant>&, const void*)>;
void RegistBuildInFunc(const std::string& name, BuildInFunc func);

}