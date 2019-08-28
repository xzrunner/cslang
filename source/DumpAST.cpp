#include "vexc/DumpAST.h"
#include "vexc/StringHelper.h"

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

}

namespace vexc
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
		output << TypeToString(expr->ty);
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
            if (expr->ty.categ == ARRAY && expr->ty.b_categ != CHAR ||
                expr->ty.categ != ARRAY && expr->ty.b_categ != CHAR) {
                output << "L";
            }
			output << "\"";

            auto str = (char*)(expr->val.p);
			for (int i = 0, n = strlen(str); i < n; ++i)
			{
                std::string s;
                if (isprint(str[i])) {
                    s = StringHelper::Format("%c", str[i]);
                } else {
                    s = StringHelper::Format("\\x%x", str[i]);
                }
                output << s;
			}

            output << "\"";
		}
		break;

	case OP_CONST:
		{
			int categ = expr->ty.categ;
            std::string s;
			if (categ == INT || categ == LONG || categ == LONGLONG) {
                s = StringHelper::Format("%d", expr->val.i[0]);
			} else if (categ == UINT || categ == ULONG || categ == ULONGLONG || categ == POINTER) {
                s = StringHelper::Format("%u", expr->val.i[0]);
			} else if (categ == FLOAT) {
                s = StringHelper::Format("%g", expr->val.f);
			} else {
                s = StringHelper::Format("%g", expr->val.d);
            }
            output << s;
		}
		break;

	default:
        output << "ERR";
		break;
	}
}

}