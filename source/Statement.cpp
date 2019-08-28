#include "vexc/Statement.h"
#include "vexc/Parser.h"
#include "vexc/Declaration.h"
#include "vexc/AST.h"
#include "vexc/grammer.h"

namespace vexc
{
namespace ast
{

static int FIRST_Statement[] = { FIRST_STATEMENT, 0};

/**
 *  compound-statement:
 *		{ [declaration-list] [statement-list] }
 *  declaration-list:
 *		declaration
 *		declaration-list declaration
 *  statement-list:
 *		statement
 *		statement-list statement
 */
StmtNodePtr StatementParser::ParseCompoundStatement(Parser& parser)
{
//	Level++;

    auto comp_stmt = std::make_shared<CompoundStmtNode>(parser.GetTokenizer(), NK_CompoundStatement);

	parser.NextToken();
    NodePtr* tail = &comp_stmt->decls;
	while (ASTHelper::CurrentTokenIn(parser.CurrTokenType(), FIRST_Declaration))
	{
        if (parser.CurrTokenType() == TK_ID &&
            !DeclarationParser::IsTypeName(parser.CurrTokenType())) {
            break;
        }
//		*tail = ParseDeclaration(parser);
		tail = &(*tail)->next;
	}
	tail = &comp_stmt->stmts;
	while (parser.CurrTokenType() != TK_RBRACE && parser.CurrTokenType() != TK_END)
	{
		*tail = ParseStatement(parser);
		tail = &(*tail)->next;
        if (parser.CurrTokenType() == TK_RBRACE) {
            break;
        }
		parser.SkipTo(FIRST_Statement);
	}
	parser.Expect(TK_RBRACE);
    parser.NextToken();

    ASTHelper::PostCheckTypedef();
//	Level--;

	return comp_stmt;
}

/**
 *  expression-statement:
 *		[expression] ;
 */
StmtNodePtr StatementParser::ParseExpressionStatement(Parser& parser)
{
    auto expr_stmt = std::make_shared<ExprStmtNode>(parser.GetTokenizer(), NK_ExpressionStatement);

	if (parser.CurrTokenType() != TK_SEMICOLON) {
        expr_stmt->expr = ExpressionParser::ParseExpression(parser);
	}
    parser.Expect(TK_SEMICOLON);
    parser.NextToken();

	return expr_stmt;
}

/**
 *  label-statement:
 *		ID : statement
 */
StmtNodePtr StatementParser::ParseLabelStatement(Parser& parser)
{
    auto t = parser.PeekToken();
	if (t.GetType() == TK_COLON)
	{
        auto label_stmt = std::make_shared<LabelStmtNode>(parser.GetTokenizer(), NK_LabelStatement);
        label_stmt->id = (char*)(parser.GetTokenizer().GetTokenVal().p);

		parser.NextToken();
		parser.NextToken();
		label_stmt->stmt = ParseStatement(parser);

		return label_stmt;
	}
	else
	{
		return ParseExpressionStatement(parser);
	}
}

/**
 *  case-statement:
 *		case constant-expression : statement
 */
StmtNodePtr StatementParser::ParseCaseStatement(Parser& parser)
{
    auto case_stmt = std::make_shared<CaseStmtNode>(parser.GetTokenizer(), NK_CaseStatement);

	parser.NextToken();
	case_stmt->expr = ExpressionParser::ParseConstantExpression(parser);
	parser.Expect(TK_COLON);
    parser.NextToken();
	case_stmt->stmt = ParseStatement(parser);

	return case_stmt;
}

/**
 *  default-statement:
 *		default : statement
 */
StmtNodePtr StatementParser::ParseDefaultStatement(Parser& parser)
{
    auto def_stmt = std::make_shared<DefaultStmtNode>(parser.GetTokenizer(), NK_DefaultStatement);

	parser.NextToken();
	parser.Expect(TK_COLON);
    parser.NextToken();
	def_stmt->stmt = ParseStatement(parser);

	return def_stmt;
}

/**
 *  if-statement:
 *		if ( expression ) statement
 *		if ( epxression ) statement else statement
 */
StmtNodePtr StatementParser::ParseIfStatement(Parser& parser)
{
    auto if_stmt = std::make_shared<IfStmtNode>(parser.GetTokenizer(), NK_IfStatement);

	parser.NextToken();
	parser.Expect(TK_LPAREN);
    parser.NextToken();
	if_stmt->expr = ExpressionParser::ParseExpression(parser);
	parser.Expect(TK_RPAREN);
    parser.NextToken();
	if_stmt->then_stmt = ParseStatement(parser);
	if (parser.CurrTokenType() == TK_ELSE)
	{
		parser.NextToken();
		if_stmt->else_stmt = ParseStatement(parser);
	}

	return if_stmt;
}

/**
 *  switch-statement:
 *		switch ( expression ) statement
 */
StmtNodePtr StatementParser::ParseSwitchStatement(Parser& parser)
{
    auto swtch_stmt = std::make_shared<SwitchStmtNode>(parser.GetTokenizer(), NK_SwitchStatement);

	parser.NextToken();
	parser.Expect(TK_LPAREN);
    parser.NextToken();
	swtch_stmt->expr = ExpressionParser::ParseExpression(parser);
	parser.Expect(TK_RPAREN);
    parser.NextToken();
	swtch_stmt->stmt = ParseStatement(parser);

	return swtch_stmt;
}

/**
 *  while-statement:
 *		while ( expression ) statement
 */
StmtNodePtr StatementParser::ParseWhileStatement(Parser& parser)
{
    auto while_stmt = std::make_shared<LoopStmtNode>(parser.GetTokenizer(), NK_WhileStatement);

	parser.NextToken();
	parser.Expect(TK_LPAREN);
    parser.NextToken();
	while_stmt->expr = ExpressionParser::ParseExpression(parser);
	parser.Expect(TK_RPAREN);
    parser.NextToken();
	while_stmt->stmt = ParseStatement(parser);

	return while_stmt;
}

/**
 *  do-statement:
 *		do statement while ( expression ) ;
 */
StmtNodePtr StatementParser::ParseDoStatement(Parser& parser)
{
    auto do_stmt = std::make_shared<LoopStmtNode>(parser.GetTokenizer(), NK_DoStatement);

	parser.NextToken();
	do_stmt->stmt = ParseStatement(parser);
	parser.Expect(TK_WHILE);
    parser.NextToken();
	parser.Expect(TK_LPAREN);
    parser.NextToken();
	do_stmt->expr= ExpressionParser::ParseExpression(parser);
	parser.Expect(TK_RPAREN);
    parser.NextToken();
	parser.Expect(TK_SEMICOLON);
    parser.NextToken();

	return do_stmt;
}

/**
 *  for-statement:
 *		for ( [expression] ; [expression] ; [expression] ) statement
 */
StmtNodePtr StatementParser::ParseForStatement(Parser& parser)
{
    auto for_stmt = std::make_shared<ForStmtNode>(parser.GetTokenizer(), NK_ForStatement);

	parser.NextToken();
	parser.Expect(TK_LPAREN);
    parser.NextToken();
	if (parser.CurrTokenType() != TK_SEMICOLON)
	{
		for_stmt->init_expr = ExpressionParser::ParseExpression(parser);
	}
	parser.Expect(TK_SEMICOLON);
    parser.NextToken();
	if (parser.CurrTokenType() != TK_SEMICOLON)
	{
		for_stmt->expr = ExpressionParser::ParseExpression(parser);
	}
	parser.Expect(TK_SEMICOLON);
    parser.NextToken();
	if (parser.CurrTokenType() != TK_RPAREN)
	{
		for_stmt->incr_expr = ExpressionParser::ParseExpression(parser);
	}
	parser.Expect(TK_RPAREN);
    parser.NextToken();
	for_stmt->stmt = ParseStatement(parser);

	return for_stmt;
}

StmtNodePtr StatementParser::ParseGotoStatement(Parser& parser)
{
    auto goto_stmt = std::make_shared<GotoStmtNode>(parser.GetTokenizer(), NK_GotoStatement);

	parser.NextToken();
	if (parser.CurrTokenType() == TK_ID)
	{
		goto_stmt->id = (char*)(parser.GetTokenizer().GetTokenVal().p);
		parser.NextToken();
		parser.Expect(TK_SEMICOLON);
        parser.NextToken();
	}
	else
	{
        assert(0);
//		Error(&TokenCoord, "Expect identifier");
		if (parser.CurrTokenType() == TK_SEMICOLON)
			parser.NextToken();
	}

	return goto_stmt;
}

/**
 *  break-statement:
 *		break ;
 */
StmtNodePtr StatementParser::ParseBreakStatement(Parser& parser)
{
    auto brk_stmt = std::make_shared<BreakStmtNode>(parser.GetTokenizer(), NK_BreakStatement);

	parser.NextToken();
	parser.Expect(TK_SEMICOLON);
    parser.NextToken();

	return brk_stmt;
}

/**
 *  continue-statement:
 *		continue ;
 */
StmtNodePtr StatementParser::ParseContinueStatement(Parser& parser)
{
    auto cont_stmt = std::make_shared<ContinueStmtNode>(parser.GetTokenizer(), NK_ContinueStatement);

	parser.NextToken();
	parser.Expect(TK_SEMICOLON);
    parser.NextToken();

	return cont_stmt;
}

/**
 *  return-statement:
 *		return [expression] ;
 */
StmtNodePtr StatementParser::ParseReturnStatement(Parser& parser)
{
    auto ret_stmt = std::make_shared<ReturnStmtNode>(parser.GetTokenizer(), NK_ReturnStatement);

	parser.NextToken();
	if (parser.CurrTokenType() != TK_SEMICOLON)
	{
		ret_stmt->expr = ExpressionParser::ParseExpression(parser);
	}
	parser.Expect(TK_SEMICOLON);
    parser.NextToken();

	return ret_stmt;
}

/**
 *  statement:
 *		expression-statement
 *		labeled-statement
 *		case-statement
 *		default-statement
 *		if-statement
 *		switch-statement
 *		while-statement
 *		do-statement
 *		for-statement
 *		goto-statement
 *		continue-statement
 *		break-statement
 *		return-statement
 *		compound-statement
 */
StmtNodePtr StatementParser::ParseStatement(Parser& parser)
{
	switch (parser.CurrTokenType())
	{
	case TK_ID:
		return ParseLabelStatement(parser);

	case TK_CASE:
		return ParseCaseStatement(parser);

	case TK_DEFAULT:
		return ParseDefaultStatement(parser);

	case TK_IF:
		return ParseIfStatement(parser);

	case TK_SWITCH:
		return ParseSwitchStatement(parser);

	case TK_WHILE:
		return ParseWhileStatement(parser);

	case TK_DO:
		return ParseDoStatement(parser);

	case TK_FOR:
		return ParseForStatement(parser);

	case TK_GOTO:
		return ParseGotoStatement(parser);

	case TK_CONTINUE:
		return ParseContinueStatement(parser);

	case TK_BREAK:
		return ParseBreakStatement(parser);

	case TK_RETURN:
		return ParseReturnStatement(parser);

	case TK_LBRACE:
		return ParseCompoundStatement(parser);

	default:
		return ParseExpressionStatement(parser);
	}
}

}
}