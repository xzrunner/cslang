#pragma once

#include "cslang/AST.h"
#include "cslang/Type.h"
#include "cslang/Tokenizer.h"

#include <array>

namespace cslang
{

class Parser;

namespace ast
{

enum OP
{
#define OPINFO(op, prec, name, func, opcode) op,
#include "cslang/op_cfg.h"
#undef OPINFO
};

struct ExpressionNode;
using ExprNodePtr = std::shared_ptr<ExpressionNode>;

struct TypeNameNode;
struct ExpressionNode : public Node
{
    ExpressionNode(const Tokenizer& lexer, NodeKind kind);
    ExpressionNode(const Coord& coord, NodeKind kind);

    std::unique_ptr<Type> ty = nullptr;
    int op      : 16;
    int isarray : 1;
    int isfunc  : 1;
    int lvalue  : 1;
    int bitfld  : 1;
    int inreg   : 1;
    int unused  : 11;
    std::array<ExprNodePtr, 2> kids;
    union Value val;

    // fixme: for TK_LPAREN and TK_SIZEOF expr, use ty?
    std::shared_ptr<TypeNameNode> tn_kid = nullptr;

}; // ExpressionNode

class ExpressionParser
{
public:
    static ExprNodePtr ParseExpression(Parser& parser);
    static ExprNodePtr ParseConstantExpression(Parser& parser);
    static ExprNodePtr ParseAssignmentExpression(Parser& parser);

private:
    static ExprNodePtr ParsePrimaryExpression(Parser& parser);
    static ExprNodePtr ParsePostfixExpression(Parser& parser);
    static ExprNodePtr ParseUnaryExpression(Parser& parser);
    static ExprNodePtr ParseBinaryExpression(Parser& parser, int prec);
    static ExprNodePtr ParseConditionalExpression(Parser& parser);

public:
    static char* OPNames[];

}; // ExpressionParser

}
}