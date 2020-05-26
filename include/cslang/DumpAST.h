#pragma once

#include "cslang/Expression.h"
#include "cslang/Statement.h"
#include "cslang/Declaration.h"

namespace cslang
{

void DumpExpression(std::ostream& output,
    const ast::ExprNodePtr& expr, int pos);

void DumpStatement(std::ostream& output,
    const ast::StmtNodePtr& stmt, int pos);

void DumpDeclaration(std::ostream& output,
    const ast::DeclarationNodePtr& decl, int pos);

void DumpTranslationUnit(std::ostream& output,
    const ast::TranslationUnitNodePtr& transUnit);

}