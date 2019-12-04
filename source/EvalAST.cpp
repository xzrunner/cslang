#include "vexc/EvalAST.h"
#include "vexc/StringPool.h"

#include <vector>
#include <map>

#define EVAL_V0                              \
auto v0 = EvalExpression(expr->kids[0], ud); \
if (v0.type == VarType::Invalid) {           \
    return Variant(VarType::Invalid);        \
}

#define EVAL_V1                              \
auto v1 = EvalExpression(expr->kids[1], ud); \
if (v1.type == VarType::Invalid) {           \
    return Variant(VarType::Invalid);        \
}

namespace
{

using namespace vexc;
using namespace vexc::ast;

std::map<std::string, BuildInFunc> FUNCS;

GetterFunc GETTER = nullptr;
SetterFunc SETTER = nullptr;

Variant EvalBuildInFunc(const ExprNodePtr& expr, const void* ud)
{
    assert(expr->op == OP_CALL);

    assert(expr->kids[0]->op == OP_ID);
    std::string func_name = (char*)(expr->kids[0]->val.p);

    auto itr = FUNCS.find(func_name);
    if (itr == FUNCS.end()) {
        return Variant();
    }

    std::vector<Variant> params;
    ExprNodePtr p = expr->kids[1];
    while (p) {
        params.push_back(EvalExpression(p, ud));
        p = std::static_pointer_cast<ExpressionNode>(p->next);
    }
    return itr->second(params, ud);
}

}

namespace vexc
{

using namespace ast;

Variant EvalExpression(const ExprNodePtr& expr, const void* ud)
{
    switch (expr->op)
    {
    case OP_COMMA:
        return EvalExpression(expr->kids[1], ud);
    case OP_ASSIGN:
    {
        EVAL_V1

        if (GETTER)
        {
            switch (expr->kids[1]->op)
            {
            case OP_ID:
                v1 = GETTER((char*)(expr->kids[1]->val.p), ud);
                break;
            }
        }

        if (SETTER)
        {
            switch (expr->kids[0]->op)
            {
            case OP_ID:
                SETTER((char*)(expr->kids[0]->val.p), nullptr, v1, ud);
                break;
            case OP_MEMBER:
                SETTER((char*)(expr->kids[0]->kids[0]->val.p), (char*)(expr->kids[0]->val.p), v1, ud);
                break;
            }
        }

        return Variant(v1.ToBool(ud));
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
        return Variant(v0.ToBool(ud) || v1.ToBool(ud));
    }
    case OP_AND:
    {
        EVAL_V0
        EVAL_V1
        return Variant(v0.ToBool(ud) && v1.ToBool(ud));
    }
    //case OP_BITOR:
    //case OP_BITXOR:
    //case OP_BITAND:
    case OP_EQUAL:
    {
        EVAL_V0
        EVAL_V1
        return Variant(abs(v0.ToDouble(ud) - v1.ToDouble(ud)) < std::numeric_limits<double>::epsilon());
    }
    case OP_UNEQUAL:
    {
        EVAL_V0
        EVAL_V1
        return Variant(abs(v0.ToDouble(ud) - v1.ToDouble(ud)) >= std::numeric_limits<double>::epsilon());
    }
    case OP_GREAT:
    {
        EVAL_V0
        EVAL_V1
        return Variant(v0.ToDouble(ud) > v1.ToDouble(ud));
    }
    case OP_LESS:
    {
        EVAL_V0
        EVAL_V1
        return Variant(v0.ToDouble(ud) < v1.ToDouble(ud));
    }
     case OP_GREAT_EQ:
    {
        EVAL_V0
        EVAL_V1
        return Variant(v0.ToDouble(ud) >= v1.ToDouble(ud));
    }
    case OP_LESS_EQ:
    {
        EVAL_V0
        EVAL_V1
        return Variant(v0.ToDouble(ud) <= v1.ToDouble(ud));
    }
    //case OP_LSHIFT:
    //case OP_RSHIFT:
    case OP_ADD:
    {
        EVAL_V0
        EVAL_V1
        if (v0.type == VarType::Double || v1.type == VarType::Double) {
            return Variant(v0.ToDouble(ud) + v1.ToDouble(ud));
        } else if (v0.type == VarType::Float || v1.type == VarType::Float) {
            return Variant(v0.ToFloat(ud) + v1.ToFloat(ud));
        } else {
            return Variant(v0.ToInt(ud) + v1.ToInt(ud));
        }
    }
    case OP_SUB:
    {
        EVAL_V0
        EVAL_V1
        if (v0.type == VarType::Double || v1.type == VarType::Double) {
            return Variant(v0.ToDouble(ud) - v1.ToDouble(ud));
        } else if (v0.type == VarType::Float || v1.type == VarType::Float) {
            return Variant(v0.ToFloat(ud) - v1.ToFloat(ud));
        } else {
            return Variant(v0.ToInt(ud) - v1.ToInt(ud));
        }
    }
    case OP_MUL:
    {
        EVAL_V0
        EVAL_V1
        if (v0.type == VarType::Double || v1.type == VarType::Double) {
            return Variant(v0.ToDouble(ud) * v1.ToDouble(ud));
        } else if (v0.type == VarType::Float || v1.type == VarType::Float) {
            return Variant(v0.ToFloat(ud) * v1.ToFloat(ud));
        } else {
            return Variant(v0.ToInt(ud) * v1.ToInt(ud));
        }
    }
    case OP_DIV:
    {
        EVAL_V0
        EVAL_V1
        auto d1 = v1.ToDouble(ud);
        if (abs(d1) < std::numeric_limits<double>::epsilon()) {
            return Variant(VarType::Invalid);
        }
        if (v0.type == VarType::Double || v1.type == VarType::Double) {
            return Variant(v0.ToDouble(ud) / v1.ToDouble(ud));
        } else if (v0.type == VarType::Float || v1.type == VarType::Float) {
            return Variant(v0.ToFloat(ud) / v1.ToFloat(ud));
        } else {
            return Variant(v0.ToInt(ud) / v1.ToInt(ud));
        }
    }
    case OP_MOD:
    {
        EVAL_V0
        EVAL_V1
        if (v0.type == VarType::Int && v1.type == VarType::Int) {
            return Variant(v0.i % v1.i);
        } else {
            return Variant(VarType::Invalid);
        }
    }
    case OP_CAST:
    {
        EVAL_V0
        switch (expr->ty->categ)
        {
        case INT:
            return Variant(v0.ToInt(ud));
        case FLOAT:
            return Variant(v0.ToInt(ud));
        case DOUBLE:
            return Variant(v0.ToDouble(ud));
        default:
            assert(0);
            return Variant();
        }
    }
        break;
    case OP_PREINC:
    {
        EVAL_V0
        if (v0.type == VarType::Int) {
            return Variant(++v0.i);
        } else {
            return Variant(VarType::Invalid);
        }
    }
    case OP_PREDEC:
    {
        EVAL_V0
        if (v0.type == VarType::Int) {
            return Variant(--v0.i);
        } else {
            return Variant(VarType::Invalid);
        }
    }
    //case OP_ADDRESS:
    //case OP_DEREF:
    case OP_POS:
        return EvalExpression(expr->kids[0], ud);
    case OP_NEG:
    {
        EVAL_V0
        switch (v0.type)
        {
        case VarType::Invalid:
            return Variant(VarType::Invalid);
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
        return Variant(!v0.ToBool(ud));
    }
    //case OP_SIZEOF:
    //case OP_INDEX:
    case OP_CALL:
        return EvalBuildInFunc(expr, ud);
    case OP_MEMBER:
    {
        EVAL_V0
        switch (v0.type)
        {
        case VarType::String:
        {
            assert(0);
            return Variant();

            std::string var = StringPool::VoidToString(v0.p);
            if (var.empty()) {
                return Variant();
            }
        }
            break;
        case VarType::Float3:
        {
            auto mem = static_cast<const char*>(expr->val.p);
            auto f3 = static_cast<const float*>(v0.p);
            if (mem[0] == 'x') {
                return Variant(f3[0]);
            } else if (mem[0] == 'y') {
                return Variant(f3[1]);
            } else if (mem[0] == 'z') {
                return Variant(f3[2]);
            } else {
                assert(0);
                return Variant();
            }
        }
            break;
        }
        return Variant();
    }
    //case OP_PTR_MEMBER:
    case OP_POSTINC:
    case OP_POSTDEC:
        return EvalExpression(expr->kids[0], ud);
    case OP_ID:
    {
        if (GETTER) {
            auto var = GETTER((char*)(expr->val.p), ud);
            if (var.type != VarType::Invalid) {
                return var;
            }
        }

        auto str = (char*)(expr->val.p);
        if (strcmp(str, "true") == 0) {
            return Variant(true);
        } else if (strcmp(str, "false") == 0) {
            return Variant(false);
        } else {
            return Variant();
        }
    }
    case OP_CONST:
    {
        int categ = expr->ty->categ;
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
    case OP_STR:
    {
        return Variant(VarType::String, expr->val.p);
    }
    //case OP_NONE:
    default:
        assert(0);
        return Variant();
    }
}

void RunStatement(const ast::StmtNodePtr& stmt, const void* ud)
{
    switch (stmt->kind)
    {
    case NK_ExpressionStatement:
        EvalExpression(std::static_pointer_cast<ExprStmtNode>(stmt)->expr, ud);
        break;

    case NK_IfStatement:
    {
        auto if_node = std::static_pointer_cast<IfStmtNode>(stmt);
        auto cond = EvalExpression(if_node->expr, ud);
        assert(cond.type == VarType::Bool);
        if (cond.b) {
            RunStatement(if_node->then_stmt, ud);
        } else if (if_node->else_stmt) {
            RunStatement(if_node->else_stmt, ud);
        }
    }
        break;

    case NK_CompoundStatement:
    {
        auto p = std::static_pointer_cast<CompoundStmtNode>(stmt)->stmts;
        while (p) {
            RunStatement(std::static_pointer_cast<StatementNode>(p), ud);
            p = p->next;
        }
    }
        break;

    default:
        assert(0);
    }
}

void RegistBuildInFunc(const std::string& name, BuildInFunc func)
{
    FUNCS.insert({ name, func });
}

void RegistGetter(GetterFunc func)
{
    GETTER = func;
}

void RegistSetter(SetterFunc func)
{
    SETTER = func;
}

}