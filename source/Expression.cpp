#include "cslang/Expression.h"
#include "cslang/Parser.h"
#include "cslang/Declaration.h"
#include "cslang/Type.h"
#include "cslang/predef.h"
#include "cslang/Utility.h"

namespace
{

using namespace cslang::ast;

struct TokenOp
{
	int bop  : 16;
	int uop  : 16;
};

struct TokenOp TokenOps[] =
{
#define TOKENOP(tok, bop, uop) {bop, uop},
#include "cslang/token_op_cfg.h"
#undef  TOKENOP
};

#define IsBinaryOP(tok) (tok >= TK_OR && tok <= TK_MOD)
#define	BINARY_OP(tok)   TokenOps[tok - TK_ASSIGN].bop
#define UNARY_OP(tok)    TokenOps[tok - TK_ASSIGN].uop

int Prec[] =
{
#define OPINFO(op, prec, name, func, opcode) prec,
#include "cslang/op_cfg.h"
	0
#undef OPINFO
};

}

namespace cslang
{
namespace ast
{

char* ExpressionParser::OPNames[] =
{
#define OPINFO(op, prec, name, func, opcode) name,
#include "cslang/op_cfg.h"
	NULL
#undef OPINFO
};

ExpressionNode::ExpressionNode(const Tokenizer& lexer, NodeKind kind)
    : Node(lexer, kind)
{
}

ExpressionNode::ExpressionNode(const Coord& coord, NodeKind kind)
    : Node(coord, kind)
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
        auto expr = std::make_shared<ExpressionNode>(parser.GetTokenizer(), NK_Expression);

        int type = 0;
        switch (parser.CurrTokenType())
        {
        case TK_INTCONST:
            type = INT;
            break;
        case TK_UINTCONST:
            type = UINT;
            break;
        case TK_LONGCONST:
            type = LONG;
            break;
        case TK_ULONGCONST:
            type = ULONG;
            break;
        case TK_LLONGCONST:
            type = LONGLONG;
            break;
        case TK_ULLONGCONST:
            type = ULONGLONG;
            break;
        case TK_FLOATCONST:
            type = FLOAT;
            break;
        case TK_DOUBLECONST:
            type = DOUBLE;
            break;
        case TK_LDOUBLECONST:
            type = LONGDOUBLE;
            break;
        default:
            assert(0);
        }
        expr->ty  = std::make_unique<Type>(Types[type]);
        expr->op  = OP_CONST;
        expr->val = parser.GetTokenizer().GetTokenVal();
        parser.NextToken();

        return expr;
    }

	case TK_STRING:
	case TK_WIDESTRING:
    {
        auto expr = std::make_shared<ExpressionNode>(parser.GetTokenizer(), NK_Expression);
        expr->ty = ArrayOf(strlen((char*)(parser.GetTokenizer().GetTokenVal().p)) + 1, parser.CurrTokenType() == TK_STRING ? Types[CHAR] : WCharType);
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
 *		type-name ( unary-expression )
 *		sizeof unary-expression
 *		sizeof ( type-name )
 *
 *  unary-operator:
 *		++ -- & * + - ! ~
 */
ExprNodePtr ExpressionParser::ParseUnaryExpression(Parser& parser)
{
    // type-name ( unary-expression )
    if (parser.IsTypeName(parser.CurrToken()))
    {
        if (parser.CurrTokenType() < TK_SIGNED) {
            parser.NextToken();
            assert(parser.IsTypeName(parser.CurrToken()));
        }

        auto expr = std::make_shared<ExpressionNode>(parser.GetTokenizer(), NK_Expression);
        expr->op = OP_CAST;

        const int type = TokenTypeToSymbolType(parser.CurrTokenType());

        //auto type_name = DeclarationParser::ParseTypeName(parser);
        //auto type_token = std::static_pointer_cast<ast::TokenNode>(type_name->specs->tySpecs)->token;
        //const int type = TokenTypeToSymbolType(type_token);

        assert(type >= 0);
        expr->ty = std::make_unique<Type>(Types[type]);
        parser.NextToken();
        expr->kids[0] = ParseUnaryExpression(parser);
        return expr;
    }

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
        // fixme: vec2(float(x),float(y))
        if (parser.IsTypeName(t))
        {
            auto expr = std::make_shared<ExpressionNode>(parser.GetTokenizer(), NK_Expression);
            expr->op = OP_CAST;
            parser.NextToken();
            const int type = TokenTypeToSymbolType(parser.CurrTokenType());
            assert(type >= 0);
            expr->ty = std::make_unique<Type>(Types[type]);
            expr->kids[0] = ParseUnaryExpression(parser);

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
            if (parser.IsTypeName(t))
            {
                parser.NextToken();

                TypeNameNodePtr tn_node = DeclarationParser::ParseTypeName(parser);
                expr->tn_kid = tn_node;

                //auto type = std::static_pointer_cast<cslang::ast::TokenNode>(expr->tn_kid->specs->tySpecs)->token;
                //expr->ty = std::make_unique<Type>(Types[type]);

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