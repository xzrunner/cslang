#pragma once

#include "vexc/Variant.h"
#include "vexc/Expression.h"
#include "vexc/Statement.h"

#include <vector>

namespace vexc
{

Variant EvalExpression(const ast::ExprNodePtr& expr, const void* ud = nullptr);
void RunStatement(const ast::StmtNodePtr& stmt, const void* ud = nullptr);

using BuildInFunc = std::function<Variant(const std::vector<Variant>&, const void*)>;
void RegistBuildInFunc(const std::string& name, BuildInFunc func);

using GetterFunc = std::function<Variant(const char*, const void* ud)>;
void RegistGetter(GetterFunc func);
using SetterFunc = std::function<void(const char*, Variant, const void* ud)>;
void RegistSetter(SetterFunc func);

}