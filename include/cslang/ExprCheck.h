#pragma once

#include "cslang/Expression.h"

namespace cslang
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