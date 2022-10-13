#pragma once

#include "cslang/Expression.h"
#include "cslang/Statement.h"
#include "cslang/Declaration.h"

namespace cslang
{

void GenExpression(std::ostream& output,
    const ast::ExprNodePtr& expr, int pos);

void GenVariant(std::ostream& output, 
    const ast::DeclarationNodePtr& decl, int pos);

void GenStatement(std::ostream& output,
    const ast::StmtNodePtr& stmt, int pos);

void GenFunction(std::ostream& output,
    const ast::FunctionNodePtr& func);

void GenTranslationUnit(std::ostream& output,
    const ast::TranslationUnitNodePtr& transUnit);

}