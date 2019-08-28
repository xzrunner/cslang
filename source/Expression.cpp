#include "vexc/Expression.h"
#include "vexc/Parser.h"
#include "vexc/Declaration.h"
#include "vexc/Type.h"

namespace
{

using namespace vexc::ast;

struct TokenOp
{
	int bop  : 16;
	int uop  : 16;
};

struct TokenOp TokenOps[] =
{
#define TOKENOP(tok, bop, uop) {bop, uop},
#include "vexc/token_op_cfg.h"
#undef  TOKENOP
};

#define IsBinaryOP(tok) (tok >= TK_OR && tok <= TK_MOD)
#define	BINARY_OP(tok)   TokenOps[tok - TK_ASSIGN].bop
#define UNARY_OP(tok)    TokenOps[tok - TK_ASSIGN].uop

int Prec[] =
{
#define OPINFO(op, prec, name, func, opcode) prec,
#include "vexc/op_cfg.h"
	0
#undef OPINFO
};

}

namespace vexc
{
namespace ast
{

char* ExpressionParser::OPNames[] =
{
#define OPINFO(op, prec, name, func, opcode) name,
#include "vexc/op_cfg.h"
	NULL
#undef OPINFO
};

ExpressionNode::ExpressionNode(const Tokenizer& lexer, NodeKind kind)
    : Node(lexer, kind)
{
}

/**
 *  expression:
 *      assignment-expression
 *      expression , assignment-expression
 */
ExprNodePtr ExpressionParser::ParseExpression(Parser& parser)
{
    auto expr = ParseAssignmentExpression(parser);
    while (parser.CurrTokenType() == TK_COMMA)
    {
        auto coma_expr = std::make_shared<ExpressionNode>(parser.GetTokenizer(), NK_Expression);
        coma_expr->op = OP_COMMA;
        coma_expr->kids[0] = expr;
        parser.NextToken();
        coma_expr->kids[1] = ParseAssignmentExpression(parser);

        expr = coma_expr;
    }

    return expr;
}

/**
 * Parse constant expression which is actually a conditional expression
 */
ExprNodePtr ExpressionParser::ParseConstantExpression(Parser& parser)
{
    return ParseConditionalExpression(parser);
}

/**
 *  assignment-expression:
 *      conditional-expression
 *      unary-expression assignment-operator assignment-expression
 *  assignment-operator:
 *      = *= /= %= += -= <<= >>= &= ^= |=
 *  There is a little twist here: the parser always treats the first nonterminal
 *  as a conditional expression.
 */
ExprNodePtr ExpressionParser::ParseAssignmentExpression(Parser& parser)
{
	auto expr = ParseConditionalExpression(parser);
    auto curr_token = parser.CurrTokenType();
	if (curr_token >= TK_ASSIGN && curr_token <= TK_MOD_ASSIGN)
	{
        auto asgn_expr = std::make_shared<ExpressionNode>(parser.GetTokenizer(), NK_Expression);
		asgn_expr->op = BINARY_OP(curr_token);
		asgn_expr->kids[0] = expr;
        parser.NextToken();
		asgn_expr->kids[1] = ParseAssignmentExpression(parser);

		return asgn_expr;
	}

	return expr;
}

/**
 *  primary-expression:
 *		ID
 *		constant
 *		string-literal
 *		( expression )
 */
ExprNodePtr ExpressionParser::ParsePrimaryExpression(Parser& parser)
{
	switch (parser.CurrTokenType())
	{
	case TK_ID:
    {
        auto expr = std::make_shared<ExpressionNode>(parser.GetTokenizer(), NK_Expression);
        expr->op = OP_ID;
        expr->val = parser.GetTokenizer().GetTokenVal();
        parser.NextToken();

        return expr;
    }

	case TK_INTCONST:
	case TK_UINTCONST:
	case TK_LONGCONST:
	case TK_ULONGCONST:
	case TK_LLONGCONST:
	case TK_ULLONGCONST:
	case TK_FLOATCONST:
	case TK_DOUBLECONST:
	case TK_LDOUBLECONST:
    {
        auto token_type = parser.CurrTokenType();
        auto expr = std::make_shared<ExpressionNode>(parser.GetTokenizer(), NK_Expression);
        if (token_type >= TK_FLOATCONST) {
            token_type++;
        }

        expr->ty  = T(INT + token_type - TK_INTCONST);
        expr->op  = OP_CONST;
        expr->val = parser.GetTokenizer().GetTokenVal();
        parser.NextToken();

        return expr;
    }

	case TK_STRING:
	case TK_WIDESTRING:
    {
        auto expr = std::make_shared<ExpressionNode>(parser.GetTokenizer(), NK_Expression);
        expr->ty = ArrayOf(strlen((char*)(parser.GetTokenizer().GetTokenVal().p)) + 1, parser.CurrTokenType() == TK_STRING ? T(CHAR) : WCharType);
        expr->op = OP_STR;
        expr->val = parser.GetTokenizer().GetTokenVal();
        parser.NextToken();

        return expr;
    }

	case TK_LPAREN:
    {
        parser.NextToken();
        auto expr = ParseExpression(parser);
        parser.Expect(TK_RPAREN);
        parser.NextToken();

        return expr;
    }

	default:
    {
        assert(0);
//        Error(&TokenCoord, "Expect identifier, string, constant or (");
        return nullptr;
    }
	}
}

/**
 *  postfix-expression:
 *		primary-expression
 *		postfix-expression [ expression ]
 *		postfix-expression ( [argument-expression-list] )
 *		postfix-expression . identifier
 *		postfix-expression -> identifier
 *		postfix-expression ++
 *		postfix-expression --
 */
ExprNodePtr ExpressionParser::ParsePostfixExpression(Parser& parser)
{
	auto expr = ParsePrimaryExpression(parser);
	while (1)
	{
		switch (parser.CurrTokenType())
		{
		case TK_LBRACKET:
        {
            auto p = std::make_shared<ExpressionNode>(parser.GetTokenizer(), NK_Expression);
            p->op = OP_INDEX;
            p->kids[0] = expr;
            parser.NextToken();
            p->kids[1] = ParseExpression(parser);
            parser.Expect(TK_RBRACKET);
            parser.NextToken();

            expr = p;
        }
			break;

		case TK_LPAREN:
        {
            auto p = std::make_shared<ExpressionNode>(parser.GetTokenizer(), NK_Expression);
            p->op = OP_CALL;
            p->kids[0] = expr;
            parser.NextToken();
            if (parser.CurrTokenType() != TK_RPAREN)
            {
                NodePtr* tail = nullptr;

                p->kids[1] = ParseAssignmentExpression(parser);
                tail = &p->kids[1]->next;
                while (parser.CurrTokenType() == TK_COMMA)
                {
                    parser.NextToken();
                    *tail = ParseAssignmentExpression(parser);
                    tail = &(*tail)->next;
                }
            }
            parser.Expect(TK_RPAREN);
            parser.NextToken();

            expr = p;
        }
			break;

		case TK_DOT:
		case TK_POINTER:
        {
            auto p = std::make_shared<ExpressionNode>(parser.GetTokenizer(), NK_Expression);
            p->op = (parser.CurrTokenType() == TK_DOT ? OP_MEMBER : OP_PTR_MEMBER);
            p->kids[0] = expr;
            parser.NextToken();
            if (parser.CurrTokenType() != TK_ID)
            {
                assert(0);
                //				Error(&p->coord, "Expect identifier as struct or union member");
            }
            else
            {
                p->val = parser.GetTokenizer().GetTokenVal();
                parser.NextToken();
            }

            expr = p;
        }
			break;

		case TK_INC:
		case TK_DEC:
        {
            auto p = std::make_shared<ExpressionNode>(parser.GetTokenizer(), NK_Expression);
            p->op = (parser.CurrTokenType() == TK_INC) ? OP_POSTINC : OP_POSTDEC;
            p->kids[0] = expr;
            parser.NextToken();

            expr = p;
        }
			break;

		default:

			return expr;
		}
	}
}

/**
 *  unary-expression:
 *		postfix-expression
 *		unary-operator unary-expression
 *		( type-name ) unary-expression
 *		sizeof unary-expression
 *		sizeof ( type-name )
 *
 *  unary-operator:
 *		++ -- & * + - ! ~
 */
ExprNodePtr ExpressionParser::ParseUnaryExpression(Parser& parser)
{
	switch (parser.CurrTokenType())
	{
	case TK_INC:
	case TK_DEC:
	case TK_BITAND:
	case TK_MUL:
	case TK_ADD:
	case TK_SUB:
	case TK_NOT:
	case TK_COMP:
    {
        auto expr = std::make_shared<ExpressionNode>(parser.GetTokenizer(), NK_Expression);
        expr->op = UNARY_OP(parser.CurrTokenType());
        parser.NextToken();
        expr->kids[0] = ParseUnaryExpression(parser);

        return expr;
    }
        break;

	case TK_LPAREN:
    {
        auto t = parser.PeekToken();
        if (DeclarationParser::IsTypeName(t.GetType()))
        {
            auto expr = std::make_shared<ExpressionNode>(parser.GetTokenizer(), NK_Expression);
            expr->op = OP_CAST;
            parser.NextToken();
//            expr->kids[0] = (AstExpression)ParseTypeName();
            parser.Expect(TK_RPAREN);
            parser.NextToken();

            expr->kids[1] = ParseUnaryExpression(parser);

            return expr;
        }
        else
        {
            return ParsePostfixExpression(parser);
        }
    }
		break;

	case TK_SIZEOF:
    {
        auto expr = std::make_shared<ExpressionNode>(parser.GetTokenizer(), NK_Expression);
        expr->op = OP_SIZEOF;
        parser.NextToken();
        if (parser.CurrTokenType() == TK_LPAREN)
        {
            auto t = parser.PeekToken();
            if (DeclarationParser::IsTypeName(t.GetType()))
            {
                parser.NextToken();
//                expr->kids[0] = (AstExpression)ParseTypeName();
                parser.Expect(TK_RPAREN);
                parser.NextToken();
            }
            else
            {
                expr->kids[0] = ParseUnaryExpression(parser);
            }
        }
        else
        {
            expr->kids[0] = ParseUnaryExpression(parser);
        }

        return expr;
    }

	default:
		return ParsePostfixExpression(parser);
	}
}

/**
 * Parse a binary expression, from logical-OR-expresssion to multiplicative-expression
 */
ExprNodePtr ExpressionParser::ParseBinaryExpression(Parser& parser, int prec)
{
    int new_prec = 0;
    auto expr = ParseUnaryExpression(parser);
    while (IsBinaryOP(parser.CurrTokenType()) && (new_prec = Prec[BINARY_OP(parser.CurrTokenType())]) >= prec)
    {
        auto bin_expr = std::make_shared<ExpressionNode>(parser.GetTokenizer(), NK_Expression);
        bin_expr->op = BINARY_OP(parser.CurrTokenType());
        bin_expr->kids[0] = expr;
        parser.NextToken();
        bin_expr->kids[1] = ParseBinaryExpression(parser, new_prec + 1);

        expr = bin_expr;
    }

    return expr;

}

/**
 *  conditional-expression:
 *      logical-OR-expression
 *      logical-OR-expression ? expression : conditional-expression
 */
ExprNodePtr ExpressionParser::ParseConditionalExpression(Parser& parser)
{
	auto expr = ParseBinaryExpression(parser, Prec[OP_OR]);
	if (parser.CurrTokenType() == TK_QUESTION)
	{
        auto cond_expr = std::make_shared<ExpressionNode>(parser.GetTokenizer(), NK_Expression);
		cond_expr->op = OP_QUESTION;
		cond_expr->kids[0] = expr;
        parser.NextToken();

        cond_expr->kids[1] = std::make_shared<ExpressionNode>(parser.GetTokenizer(), NK_Expression);
		cond_expr->kids[1]->op = OP_COLON;
		cond_expr->kids[1]->kids[0] = ParseExpression(parser);
        parser.Expect(TK_COLON);
        parser.NextToken();
		cond_expr->kids[1]->kids[1] = ParseConditionalExpression(parser);

		return cond_expr;
	}

	return expr;
}

}
}