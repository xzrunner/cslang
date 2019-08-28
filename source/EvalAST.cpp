#include "vexc/EvalAST.h"

#include <vector>

#define EVAL_V0                          \
auto v0 = EvalExpression(expr->kids[0]); \
if (v0.type == VarType::NaN) {           \
    return Variant(VarType::NaN);        \
}

#define EVAL_V1                          \
auto v1 = EvalExpression(expr->kids[1]); \
if (v1.type == VarType::NaN) {           \
    return Variant(VarType::NaN);        \
}

namespace vexc
{

using namespace ast;

Variant EvalExpression(const ast::ExprNodePtr& expr)
{
    switch (expr->op)
    {
    case OP_COMMA:
        return EvalExpression(expr->kids[1]);
    case OP_ASSIGN:
    {
        EVAL_V1
        return Variant(v1.ToBool());
    }
    //case OP_BITOR_ASSIGN:
    //case OP_BITXOR_ASSIGN:
    //case OP_BITAND_ASSIGN:
    //case OP_LSHIFT_ASSIGN:
    //case OP_RSHIFT_ASSIGN:
    //case OP_ADD_ASSIGN:
    //case OP_SUB_ASSIGN:
    //case OP_MUL_ASSIGN:
    //case OP_DIV_ASSIGN:
    //case OP_MOD_ASSIGN:
    //case OP_QUESTION:
    //case OP_COLON:
    case OP_OR:
    {
        EVAL_V0
        EVAL_V1
        return Variant(v0.ToBool() || v1.ToBool());
    }
    case OP_AND:
    {
        EVAL_V0
        EVAL_V1
        return Variant(v0.ToBool() && v1.ToBool());
    }
    //case OP_BITOR:
    //case OP_BITXOR:
    //case OP_BITAND:
    case OP_EQUAL:
    {
        EVAL_V0
        EVAL_V1
        return Variant(abs(v0.ToDouble() - v1.ToDouble()) < std::numeric_limits<double>::epsilon());
    }
    case OP_UNEQUAL:
    {
        EVAL_V0
        EVAL_V1
        return Variant(abs(v0.ToDouble() - v1.ToDouble()) >= std::numeric_limits<double>::epsilon());
    }
    case OP_GREAT:
    {
        EVAL_V0
        EVAL_V1
        return Variant(v0.ToDouble() > v1.ToDouble());
    }
    case OP_LESS:
    {
        EVAL_V0
        EVAL_V1
        return Variant(v0.ToDouble() < v1.ToDouble());
    }
    case OP_GREAT_EQ:
    {
        EVAL_V0
        EVAL_V1
        return Variant(v0.ToDouble() >= v1.ToDouble());
    }
    case OP_LESS_EQ:
    {
        EVAL_V0
        EVAL_V1
        return Variant(v0.ToDouble() <= v1.ToDouble());
    }
    //case OP_LSHIFT:
    //case OP_RSHIFT:
    case OP_ADD:
    {
        EVAL_V0
        EVAL_V1
        if (v0.type == VarType::Double || v1.type == VarType::Double) {
            return Variant(v0.ToDouble() + v1.ToDouble());
        } else if (v0.type == VarType::Float || v1.type == VarType::Float) {
            return Variant(v0.ToFloat() + v1.ToFloat());
        } else {
            return Variant(v0.ToInt() + v1.ToInt());
        }
    }
    case OP_SUB:
    {
        EVAL_V0
        EVAL_V1
        if (v0.type == VarType::Double || v1.type == VarType::Double) {
            return Variant(v0.ToDouble() - v1.ToDouble());
        } else if (v0.type == VarType::Float || v1.type == VarType::Float) {
            return Variant(v0.ToFloat() - v1.ToFloat());
        } else {
            return Variant(v0.ToInt() - v1.ToInt());
        }
    }
    case OP_MUL:
    {
        EVAL_V0
        EVAL_V1
        if (v0.type == VarType::Double || v1.type == VarType::Double) {
            return Variant(v0.ToDouble() * v1.ToDouble());
        } else if (v0.type == VarType::Float || v1.type == VarType::Float) {
            return Variant(v0.ToFloat() * v1.ToFloat());
        } else {
            return Variant(v0.ToInt() * v1.ToInt());
        }
    }
    case OP_DIV:
    {
        EVAL_V0
        EVAL_V1
        auto d1 = v1.ToDouble();
        if (abs(d1) < std::numeric_limits<double>::epsilon()) {
            return Variant(VarType::NaN);
        }
        if (v0.type == VarType::Double || v1.type == VarType::Double) {
            return Variant(v0.ToDouble() / v1.ToDouble());
        } else if (v0.type == VarType::Float || v1.type == VarType::Float) {
            return Variant(v0.ToFloat() / v1.ToFloat());
        } else {
            return Variant(v0.ToInt() / v1.ToInt());
        }
    }
    case OP_MOD:
    {
        EVAL_V0
        EVAL_V1
        if (v0.type == VarType::Int && v1.type == VarType::Int) {
            return Variant(v0.i % v1.i);
        } else {
            return Variant(VarType::NaN);
        }
    }
    //case OP_CAST:
    case OP_PREINC:
    {
        EVAL_V0
        if (v0.type == VarType::Int) {
            return Variant(++v0.i);
        } else {
            return Variant(VarType::NaN);
        }
    }
    case OP_PREDEC:
    {
        EVAL_V0
        if (v0.type == VarType::Int) {
            return Variant(--v0.i);
        } else {
            return Variant(VarType::NaN);
        }
    }
    //case OP_ADDRESS:
    //case OP_DEREF:
    case OP_POS:
        return EvalExpression(expr->kids[0]);
    case OP_NEG:
    {
        EVAL_V0
        switch (v0.type)
        {
        case VarType::NaN:
            return Variant(VarType::NaN);
        case VarType::Bool:
            return Variant(!v0.b);
        case VarType::Int:
            return Variant(-v0.i);
        case VarType::Float:
            return Variant(-v0.f);
        case VarType::Double:
            return Variant(-v0.d);
        default:
            assert(0);
        }
    }
    //case OP_COMP:
    case OP_NOT:
    {
        EVAL_V0
        return Variant(!v0.ToBool());
    }
    //case OP_SIZEOF:
    //case OP_INDEX:
    case OP_CALL:
        return EvalBuildInFunc(expr);
    //case OP_MEMBER:
    //case OP_PTR_MEMBER:
    case OP_POSTINC:
    case OP_POSTDEC:
        return EvalExpression(expr->kids[0]);
    case OP_ID:
    {
        auto str = (char*)(expr->val.p);
        if (strcmp(str, "true") == 0) {
            return Variant(true);
        } else if (strcmp(str, "false") == 0) {
            return Variant(false);
        } else {
            assert(0);
            return Variant(false);
        }
    }
    case OP_CONST:
    {
        int categ = expr->ty.categ;
        if (categ == CHAR || categ == INT || categ == LONG || categ == LONGLONG) {
            return Variant(expr->val.i[0]);
        } else if (categ == UINT || categ == ULONG || categ == ULONGLONG || categ == POINTER) {
            return Variant(/*(unsigned int)*/(expr->val.i[0]));
        } else if (categ == FLOAT) {
            return Variant(expr->val.f);
        } else {
            return Variant(expr->val.d);
        }
    }
    //case OP_STR:
    //case OP_NONE:
    default:
        assert(0);
        return Variant(false);
    }
}

Variant EvalBuildInFunc(const ast::ExprNodePtr& expr)
{
    assert(expr->op == OP_CALL);

    assert(expr->kids[0]->op == OP_ID);
    std::string func_name = (char*)(expr->kids[0]->val.p);

    std::vector<Variant> params;
    ExprNodePtr p = expr->kids[1];
    while (p) {
        params.push_back(EvalExpression(p));
        p = std::static_pointer_cast<ExpressionNode>(p->next);
    }

    if (func_name == "min")
    {

    }
    else if (func_name == "min")
    {

    }
    else if (func_name == "floor")
    {
        if (params.empty()) {
            return Variant(false);
        }

        auto& p = params[0];
        if (p.type == VarType::Float) {
            return Variant(std::floor(p.f));
        } else if (p.type == VarType::Double) {
            return Variant(std::floor(p.d));
        } else {
            return p;
        }
    }

    return Variant(false);
}

}