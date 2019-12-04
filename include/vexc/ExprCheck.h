#pragma once

#include "vexc/Expression.h"

namespace vexc
{

class Parser;

namespace ast
{

class ExpressionCheck
{
public:
    static ExprNodePtr Cast(Parser& parser, const Type& ty, ExprNodePtr expr);

private:
    static ExprNodePtr CastExpression(Parser& parser, const Type& ty, ExprNodePtr expr);
    static ExprNodePtr FoldCast(const Type& ty, ExprNodePtr expr);

}; // ExpressionCheck

}
}