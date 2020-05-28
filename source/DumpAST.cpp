#include "cslang/DumpAST.h"
#include "cslang/Utility.h"

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

void DumpExpression(std::ostream& output, const ast::ExprNodePtr& expr, int pos)
{
	if (expr == NULL)
	{
		output << "NIL";
		return;
	}

    char *opname = NULL;
	if (expr->op >= OP_COMMA && expr->op <= OP_POSTDEC) {
		opname = ExpressionParser::OPNames[expr->op];
	}

	switch (expr->op)
	{
	case OP_COMMA:
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
	case OP_INDEX:
        output << "(" << opname << " ";
		pos += strlen(opname) + 2;
		DumpExpression(output, expr->kids[0], pos);
		LeftAlign(output, pos);
		DumpExpression(output, expr->kids[1], pos);
        output << ")";
		break;

	case OP_CALL:
    {
        ExprNodePtr p = expr->kids[1];

        output << "(" << opname << " ";
        pos += strlen(opname) + 2;
        DumpExpression(output, expr->kids[0], pos);
        while (p)
        {
            LeftAlign(output, pos);
            DumpExpression(output, p, pos);
            if (p->next != nullptr) {
                output << ",";
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
		DumpExpression(output, expr->kids[0], pos);
		output << ")";
		break;

	case OP_CAST:
		output << "(" << opname << " ";
		pos += strlen(opname) + 2;
		output << TypeToString(*expr->ty);
		LeftAlign(output, pos);
		DumpExpression(output, expr->kids[0], pos);
		output << ")";
		break;

	case OP_SIZEOF:
		output << "(" << opname << " ";
		pos += strlen(opname) + 2;
		if (expr->kids[0]->kind == NK_Expression)
		{
			DumpExpression(output, expr->kids[0], pos);
		}
        output << ")";
		break;

	case OP_MEMBER:
	case OP_PTR_MEMBER:
		output << "(" << opname << " ";
		pos += strlen(opname) + 2;
		DumpExpression(output, expr->kids[0], pos);
		LeftAlign(output, pos);
        output << (char*)(expr->val.p);
		break;

	case OP_QUESTION:
        output << "(" << opname << " ";
		pos += strlen(opname) + 2;;
		DumpExpression(output, expr->kids[0], pos);
		LeftAlign(output, pos);
		DumpExpression(output, expr->kids[1]->kids[0], pos);
		LeftAlign(output, pos);
		DumpExpression(output, expr->kids[1]->kids[1], pos);
        output << ")";
		break;

	case OP_POSTINC:
	case OP_POSTDEC:
		DumpExpression(output, expr->kids[0], pos);
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

void DumpStatement(std::ostream& output, const ast::StmtNodePtr& stmt, int pos)
{
	switch (stmt->kind)
	{
	case NK_ExpressionStatement:

		DumpExpression(output, std::static_pointer_cast<ExprStmtNode>(stmt)->expr, pos);
		break;

	case NK_LabelStatement:

		print(output, "(label %s:\n", std::static_pointer_cast<LabelStmtNode>(stmt)->id);
		LeftAlign(output, pos + 2);
		DumpStatement(output, std::static_pointer_cast<LabelStmtNode>(stmt)->stmt, pos + 2);
		LeftAlign(output, pos);
		print(output, "end-label)");
		break;

	case NK_CaseStatement:

		print(output, "(case  ");
		DumpExpression(output, std::static_pointer_cast<CaseStmtNode>(stmt)->expr, pos + 7);
		LeftAlign(output, pos + 2);
		DumpStatement(output, std::static_pointer_cast<CaseStmtNode>(stmt)->stmt, pos + 2);
		LeftAlign(output, pos);
		print(output, "end-case)");
		break;

	case NK_DefaultStatement:

		print(output, "(default  ");
		DumpStatement(output, std::static_pointer_cast<DefaultStmtNode>(stmt)->stmt, pos + 10);
		LeftAlign(output, pos);
		print(output, "end-default)");
		break;

	case NK_IfStatement:

		print(output, "(if  ");
		DumpExpression(output, std::static_pointer_cast<IfStmtNode>(stmt)->expr, pos + 5);
		LeftAlign(output, pos + 2);
		print(output, "(then  ");
		LeftAlign(output, pos + 4);
		DumpStatement(output, std::static_pointer_cast<IfStmtNode>(stmt)->then_stmt, pos + 4);
		LeftAlign(output, pos + 2);
		print(output, "end-then)");
		if (std::static_pointer_cast<IfStmtNode>(stmt)->else_stmt != NULL)
		{
			LeftAlign(output, pos + 2);
			print(output, "(else  ");
			LeftAlign(output, pos + 4);
			DumpStatement(output, std::static_pointer_cast<IfStmtNode>(stmt)->else_stmt, pos + 4);
			LeftAlign(output, pos + 2);
			print(output, "end-else)");
		}
		LeftAlign(output, pos);
		print(output, "end-if)");
		break;

	case NK_SwitchStatement:

		print(output, "(switch ");
		DumpExpression(output, std::static_pointer_cast<SwitchStmtNode>(stmt)->expr, pos + 9);
		LeftAlign(output, pos + 2);
		DumpStatement(output, std::static_pointer_cast<SwitchStmtNode>(stmt)->stmt, pos + 2);
		LeftAlign(output, pos);
		print(output, "end-switch)");
		break;

	case NK_WhileStatement:

		print(output, "(while  ");
		DumpExpression(output, std::static_pointer_cast<LoopStmtNode>(stmt)->expr, pos + 8);
		LeftAlign(output, pos + 2);
		DumpStatement(output, std::static_pointer_cast<LoopStmtNode>(stmt)->stmt, pos + 2);
		LeftAlign(output, pos);
		print(output, "end-while)");
		break;

	case NK_DoStatement:

		print(output, "(do  ");
		DumpExpression(output, std::static_pointer_cast<LoopStmtNode>(stmt)->expr, pos + 5);
		LeftAlign(output, pos + 2);
		DumpStatement(output, std::static_pointer_cast<LoopStmtNode>(stmt)->stmt, pos + 2);
		LeftAlign(output, pos);
		print(output, ")");
		break;

	case NK_ForStatement:

		print(output, "(for  ");
        // todo
		//DumpExpression(output, std::static_pointer_cast<ForStmtNode>(stmt)->init_expr, pos + 6);
		LeftAlign(output, pos + 6);
		DumpExpression(output, std::static_pointer_cast<ForStmtNode>(stmt)->expr, pos + 6);
		LeftAlign(output, pos + 6);
		DumpExpression(output, std::static_pointer_cast<ForStmtNode>(stmt)->incr_expr, pos + 6);
		LeftAlign(output, pos + 2);
		DumpStatement(output, std::static_pointer_cast<ForStmtNode>(stmt)->stmt, pos + 2);
		LeftAlign(output, pos);
		print(output, "end-for)");
		break;

	case NK_GotoStatement:

		print(output, "(goto %s)", std::static_pointer_cast<GotoStmtNode>(stmt)->id);
		break;

	case NK_ContinueStatement:

		print(output, "(continue)");
		break;

	case NK_BreakStatement:

		print(output, "(break)");
		break;

	case NK_ReturnStatement:

		print(output, "(ret ");
		DumpExpression(output, std::static_pointer_cast<ReturnStmtNode>(stmt)->expr, pos + 5);
		print(output, ")");
		break;

	case NK_CompoundStatement:
		{
			auto p = std::static_pointer_cast<CompoundStmtNode>(stmt)->stmts;

			print(output, "{");
			while (p != NULL)
			{
				LeftAlign(output, pos + 2);
				DumpStatement(output, std::static_pointer_cast<StatementNode>(p), pos + 2);
				if (p->next != NULL)
					print(output, "\n");
				p = p->next;
			}
			LeftAlign(output, pos);
			print(output, "}");
			break;
		}

	default:
		assert(0);
	}
}

void DumpDeclaration(std::ostream& output, const ast::DeclarationNodePtr& decl, int pos)
{

}

void DumpFunction(std::ostream& output, const ast::FunctionNodePtr& func)
{
    print(output, "function %s\n", func->fdec->dec->id);
    DumpStatement(output, func->stmt, 0);
    print(output, "\n\n");
}

void DumpTranslationUnit(std::ostream& output, const TranslationUnitNodePtr& transUnit)
{
	auto p = transUnit->extDecls;
	while (p)
	{
		if (p->kind == NK_Function) {
			DumpFunction(output, std::static_pointer_cast<FunctionNode>(p));
		}
		p = p->next;
	}
}

}