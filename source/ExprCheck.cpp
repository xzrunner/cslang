#include "cslang/ExprCheck.h"
#include "cslang/Type.h"
#include "cslang/Parser.h"

namespace cslang
{
namespace ast
{

ExprNodePtr ExpressionCheck::Cast(Parser& parser, const Type& ty, ExprNodePtr expr)
{
	int scode = TypeCode(*expr->ty);
	int dcode = TypeCode(ty);

	if (dcode == V)
	{
		return CastExpression(parser, ty, expr);
	}

	if (scode < F4 && dcode < F4 && scode / 2 == dcode / 2)
	{
		expr->ty = std::make_unique<Type>(ty);
		return expr;
	}

	if (scode < I4)
	{
		expr = CastExpression(parser, T(INT), expr);
		scode = I4;
	}
	if (scode != dcode)
	{
		if (dcode < I4)
		{
			expr = CastExpression(parser, T(INT), expr);
		}
		expr = CastExpression(parser, ty, expr);
	}
	return expr;
}

ExprNodePtr ExpressionCheck::CastExpression(Parser& parser, const Type& ty, ExprNodePtr expr)
{
    if (expr->op == OP_CONST && ty.categ != VOID) {
        return FoldCast(ty, expr);
    }

    auto cast = std::make_shared<ExpressionNode>(parser.GetTokenizer(), NK_Expression);

	cast->coord = expr->coord;
	cast->op = OP_CAST;
	cast->ty = std::make_unique<Type>(ty);
	cast->kids[0] = expr;

	return cast;
}

ExprNodePtr ExpressionCheck::FoldCast(const Type& ty, ExprNodePtr expr)
{
	int scode = TypeCode(*expr->ty);
	int dcode = TypeCode(ty);

	switch (scode)
	{
	case I1: case U1: case I2: case U2:
		break;

	case I4:
		if (dcode == F4)
		{
			expr->val.f = (float)expr->val.i[0];
		}
		else if (dcode == F8)
		{
			expr->val.d = (double)expr->val.i[0];
		}
		else if (dcode == I1)
		{
			expr->val.i[0] = (char)expr->val.i[0];
		}
		else if (dcode == U1)
		{
			expr->val.i[0] = (unsigned char)expr->val.i[0];
		}
		else if (dcode == I2)
		{
			expr->val.i[0] = (short)expr->val.i[0];
		}
		else if (dcode == U2)
		{
			expr->val.i[0] = (unsigned short)expr->val.i[0];
		}
		break;

	case U4:
		if (dcode == F4)
		{
			expr->val.f = (float)(unsigned)expr->val.i[0];
		}
		else if (dcode == F8)
		{
			expr->val.d = (double)(unsigned)expr->val.i[0];
		}
		break;

	case F4:
		if (dcode == I4)
		{
			expr->val.i[0] = (int)expr->val.f;
		}
		else if (dcode == U4)
		{
			expr->val.i[0] = (unsigned)expr->val.f;
		}
		else
		{
			expr->val.d = (double)expr->val.f;
		}
		break;

	case F8:
		if (dcode == I4)
		{
			expr->val.i[0] = (int)expr->val.d;
		}
		else if (dcode == U4)
		{
			expr->val.i[0] = (unsigned)expr->val.d;
		}
		else
		{
			expr->val.f = (float)expr->val.d;
		}
		break;

	default:
		assert(0);
	}

	expr->ty = std::make_unique<Type>(ty);
	return expr;
}

}
}