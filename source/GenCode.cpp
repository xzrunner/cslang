#include "cslang/GenCode.h"
#include "cslang/Utility.h"
#include "cslang/Tokenizer.h"

#include <stdarg.h>

namespace
{

void LeftAlign(std::ostream& output, int pos)
{
	char spaces[256];

	pos = pos >= 256 ? 2 : pos;
	memset(spaces, ' ', pos);
	spaces[pos] = '\0';
    output << "\n" << spaces;
}

#define print(out, ...)                   \
    do {                                  \
        out << StringFormat(__VA_ARGS__); \
    } while (0)

}

namespace cslang
{

using namespace ast;

void GenExpression(std::ostream& output, const ast::ExprNodePtr& expr, int pos)
{
	if (expr == NULL)
	{
		//output << "NIL";
		return;
	}

    char *opname = NULL;
	if (expr->op >= OP_COMMA && expr->op <= OP_POSTDEC) {
		opname = ExpressionParser::OPNames[expr->op];
	}

	switch (expr->op)
	{
	case OP_COMMA:
        //output << "(";
        GenExpression(output, expr->kids[0], pos);
        output << opname << " ";
        GenExpression(output, expr->kids[1], pos);
        //output << ")";
        break;
	case OP_ASSIGN:
	case OP_BITOR_ASSIGN:
	case OP_BITXOR_ASSIGN:
	case OP_BITAND_ASSIGN:
	case OP_LSHIFT_ASSIGN:
	case OP_RSHIFT_ASSIGN:
	case OP_ADD_ASSIGN:
	case OP_SUB_ASSIGN:
	case OP_MUL_ASSIGN:
	case OP_DIV_ASSIGN:
	case OP_MOD_ASSIGN:
        GenExpression(output, expr->kids[0], pos);
        output << " " << opname << " ";
        GenExpression(output, expr->kids[1], pos);
        break;
	case OP_OR:
	case OP_AND:
	case OP_BITOR:
	case OP_BITXOR:
	case OP_BITAND:
	case OP_EQUAL:
	case OP_UNEQUAL:
	case OP_GREAT:
	case OP_LESS:
	case OP_GREAT_EQ:
	case OP_LESS_EQ:
	case OP_LSHIFT:
	case OP_RSHIFT:
	case OP_ADD:
	case OP_SUB:
	case OP_MUL:
	case OP_DIV:
	case OP_MOD:
        output << "(";
		GenExpression(output, expr->kids[0], pos);
        output << " " << opname << " ";
		GenExpression(output, expr->kids[1], pos);
        output << ")";
		break;
    case OP_INDEX:
        GenExpression(output, expr->kids[0], pos);
        output << "[";
        GenExpression(output, expr->kids[1], pos);
        output << "]";
        break;

	case OP_CALL:
    {
        ExprNodePtr p = expr->kids[1];

        GenExpression(output, expr->kids[0], pos);
        output << "(";
        while (p)
        {
            GenExpression(output, p, pos);
            if (p->next != nullptr) {
                output << ", ";
            }
            p = std::static_pointer_cast<ExpressionNode>(p->next);
        }
        output << ")";
    }
		break;

	case OP_PREINC:
	case OP_PREDEC:
	case OP_POS:
	case OP_NEG:
	case OP_NOT:
	case OP_COMP:
	case OP_ADDRESS:
	case OP_DEREF:
		output << "(" << opname << " ";
		pos += strlen(opname) + 2;
		GenExpression(output, expr->kids[0], pos);
		output << ")";
		break;

	case OP_CAST:
		output << TypeToString(*expr->ty) << "(";
		GenExpression(output, expr->kids[0], pos);
		output << ")";
		break;

	case OP_SIZEOF:
		output << "(" << opname << " ";
		pos += strlen(opname) + 2;
		if (expr->kids[0]->kind == NK_Expression)
		{
			GenExpression(output, expr->kids[0], pos);
		}
        output << ")";
		break;

	case OP_MEMBER:
	case OP_PTR_MEMBER:
		GenExpression(output, expr->kids[0], pos);
        output << "." << (char*)(expr->val.p);
		break;

	case OP_QUESTION:
        output << "(";
		GenExpression(output, expr->kids[0], pos);
        output << " " << opname << " ";
		GenExpression(output, expr->kids[1]->kids[0], pos);
        output << " : ";
		GenExpression(output, expr->kids[1]->kids[1], pos);
        output << ")";
		break;

	case OP_POSTINC:
	case OP_POSTDEC:
		GenExpression(output, expr->kids[0], pos);
        output << opname;
		break;

	case OP_ID:
        output << (char*)(expr->val.p);
		break;

	case OP_STR:
		{
            assert(0);
            //if (expr->ty->categ == ARRAY && expr->ty.b_categ != CHAR ||
            //    expr->ty->categ != ARRAY && expr->ty.b_categ != CHAR) {
            //    output << "L";
            //}
			output << "\"";

            auto str = (char*)(expr->val.p);
			for (int i = 0, n = strlen(str); i < n; ++i)
			{
                std::string s;
                if (isprint(str[i])) {
                    s = StringFormat("%c", str[i]);
                } else {
                    s = StringFormat("\\x%x", str[i]);
                }
                output << s;
			}

            output << "\"";
		}
		break;

	case OP_CONST:
		{
			int categ = expr->ty->categ;
            std::string s;
			if (categ == INT || categ == LONG || categ == LONGLONG) {
                s = StringFormat("%d", expr->val.i[0]);
			} else if (categ == UINT || categ == ULONG || categ == ULONGLONG || categ == POINTER) {
                s = StringFormat("%u", expr->val.i[0]);
			} else if (categ == FLOAT) {
                s = PrintFloatConstant(expr->val.f);
			} else {
                s = PrintDoubleConstant(expr->val.d);
            }
            output << s;
		}
		break;

	default:
        output << "ERR";
		break;
	}
}

void GenVariant(std::ostream& output, const ast::DeclarationNodePtr& decl, int pos)
{
    if (!decl) {
        output << ";";
        return;
    }

    if (decl->specs->tyQuals)
    {
        auto storage = std::static_pointer_cast<ast::TokenNode>(decl->specs->tyQuals)->token;
        output << Tokenizer::TokenStrings[storage];

        output << " ";
    }

    if (decl->specs->tySpecs)
    {
        auto type = std::static_pointer_cast<ast::TokenNode>(decl->specs->tySpecs)->token;
        output << Tokenizer::TokenStrings[type];

        output << " ";
    }

    InitDeclaratorNodePtr initDec = std::static_pointer_cast<InitDeclaratorNode>(decl->initDecs);
    while (initDec)
    {
        switch (initDec->dec->kind)
        {
        case NK_NameDeclarator:
            output << initDec->dec->id;
            break;

        case NK_ArrayDeclarator:
        {
            auto decl = std::static_pointer_cast<ArrayDeclaratorNode>(initDec->dec);
            output << decl->dec->id;
            assert(decl->expr->op == OP_CONST
                && IsIntegType(decl->expr->ty));
            output << "[" << decl->expr->val.i[0] << "]";
        }
            break;

        //case NK_FunctionDeclarator:
        //    CheckFunctionDeclarator((AstFunctionDeclarator)dec);
        //    break;

        //case NK_PointerDeclarator:
        //    CheckPointerDeclarator((AstPointerDeclarator)dec);
        //    break;

        default:
            assert(0);
        }

        if (initDec->init)
        {
            assert(initDec->init->lbrace == 0);
            output << " = ";
            GenExpression(output, initDec->init->expr, pos);
        }

        initDec = std::static_pointer_cast<InitDeclaratorNode>(initDec->next);
        if (initDec) {
            output << ", ";
        }
    }
    output << ";";
}

void GenStatement(std::ostream& output, const ast::StmtNodePtr& stmt, int pos)
{
	switch (stmt->kind)
	{
	case NK_ExpressionStatement:

		GenExpression(output, std::static_pointer_cast<ExprStmtNode>(stmt)->expr, pos);
        output << ";";
		break;

	case NK_LabelStatement:

		print(output, "(label %s:\n", std::static_pointer_cast<LabelStmtNode>(stmt)->id);
		LeftAlign(output, pos + 4);
		GenStatement(output, std::static_pointer_cast<LabelStmtNode>(stmt)->stmt, pos + 4);
		LeftAlign(output, pos);
		print(output, "end-label)");
		break;

	case NK_CaseStatement:

        output << "case ";
		GenExpression(output, std::static_pointer_cast<CaseStmtNode>(stmt)->expr, pos);
        output << ":";
        LeftAlign(output, pos + 4);
		GenStatement(output, std::static_pointer_cast<CaseStmtNode>(stmt)->stmt, pos + 4);
		break;

	case NK_DefaultStatement:

        output << "default:";
        LeftAlign(output, pos + 4);
		GenStatement(output, std::static_pointer_cast<DefaultStmtNode>(stmt)->stmt, pos + 4);
		break;

	case NK_IfStatement:

		output << "if (";
		GenExpression(output, std::static_pointer_cast<IfStmtNode>(stmt)->expr, pos);
        output << ") ";
		GenStatement(output, std::static_pointer_cast<IfStmtNode>(stmt)->then_stmt, pos);
		if (std::static_pointer_cast<IfStmtNode>(stmt)->else_stmt != NULL)
		{
            output << " else ";
			GenStatement(output, std::static_pointer_cast<IfStmtNode>(stmt)->else_stmt, pos);
		}
		break;

	case NK_SwitchStatement:

        output << "switch (";
		GenExpression(output, std::static_pointer_cast<SwitchStmtNode>(stmt)->expr, pos);
        output << ")";
        LeftAlign(output, pos);
		GenStatement(output, std::static_pointer_cast<SwitchStmtNode>(stmt)->stmt, pos);
		break;

	case NK_WhileStatement:

        output << "while (";
		GenExpression(output, std::static_pointer_cast<LoopStmtNode>(stmt)->expr, pos);
        output << ")";
        LeftAlign(output, pos);
		GenStatement(output, std::static_pointer_cast<LoopStmtNode>(stmt)->stmt, pos);
		break;

	case NK_DoStatement:

        output << "do ";
        GenStatement(output, std::static_pointer_cast<LoopStmtNode>(stmt)->stmt, pos);
        output << " while (";
		GenExpression(output, std::static_pointer_cast<LoopStmtNode>(stmt)->expr, pos);
        output << ");";
		break;

	case NK_ForStatement:

		output << "for (";
        GenVariant(output, std::static_pointer_cast<ForStmtNode>(stmt)->init_decl, pos);
        output << " ";
		GenExpression(output, std::static_pointer_cast<ForStmtNode>(stmt)->expr, pos + 6);
        output << "; ";
		GenExpression(output, std::static_pointer_cast<ForStmtNode>(stmt)->incr_expr, pos + 6);
        output << ")";
        LeftAlign(output, pos);
		GenStatement(output, std::static_pointer_cast<ForStmtNode>(stmt)->stmt, pos);
		break;

	case NK_GotoStatement:

		print(output, "goto %s;", std::static_pointer_cast<GotoStmtNode>(stmt)->id);
		break;

	case NK_ContinueStatement:

        output << "continue;";
		break;

	case NK_BreakStatement:

        output << "break;";
		break;

	case NK_ReturnStatement:

        output << "return ";
		GenExpression(output, std::static_pointer_cast<ReturnStmtNode>(stmt)->expr, pos + 5);
        output << ";";

		break;

	case NK_CompoundStatement:
		{
			auto p = std::static_pointer_cast<CompoundStmtNode>(stmt)->stmts;

			print(output, "{");
			while (p != NULL)
			{
				LeftAlign(output, pos + 4);
				GenStatement(output, std::static_pointer_cast<StatementNode>(p), pos + 4);
				//if (p->next != NULL)
				//	print(output, "\n");
				p = p->next;
			}
			LeftAlign(output, pos);
			print(output, "}");
			break;
		}

    case NK_Declaration:
        GenVariant(output, std::reinterpret_pointer_cast<DeclarationNode>(stmt), pos);
        break;

	default:
		assert(0);
	}
}

void GenFunction(std::ostream& output, const ast::FunctionNodePtr& func)
{
    // return type
    auto ret_token = std::static_pointer_cast<ast::TokenNode>(func->specs->tySpecs)->token;
    output << Tokenizer::TokenStrings[ret_token];

    // func name
    output << " " << func->fdec->dec->id << "(";

    // params
    if (func->fdec->paramTyList)
    {
        NodePtr p = func->fdec->paramTyList->paramDecls;
        while (p)
        {
            auto p_decl = std::static_pointer_cast<ParameterDeclarationNode>(p);
            auto p_token = std::static_pointer_cast<ast::TokenNode>(p_decl->specs->tySpecs)->token;
            auto p_id = p_decl->dec->id;
            output << Tokenizer::TokenStrings[p_token] << " " << p_id;
            if (p->next) {
                output << ", ";
            }
            p = p->next;
        }
    }
    output << ")\n";

    GenStatement(output, func->stmt, 0);
    output << "\n\n";
}

void GenTranslationUnit(std::ostream& output, const TranslationUnitNodePtr& transUnit)
{
	auto p = transUnit->extDecls;
	while (p)
	{
        switch (p->kind)
        {
        case NK_Declaration:
            GenVariant(output, std::static_pointer_cast<DeclarationNode>(p), 0);
            LeftAlign(output, 0);
            break;
        case NK_Function:
            GenFunction(output, std::static_pointer_cast<FunctionNode>(p));
            break;
        }
		p = p->next;
	}
}

}