#pragma once

#include "cslang/predef.h"
#include "cslang/AST.h"
#include "cslang/Expression.h"

#include <memory>

namespace cslang
{
namespace ast
{

struct StatementNode : public Node
{
    StatementNode(const Tokenizer& lexer, NodeKind kind)
        : Node(lexer, kind) {}

}; // StatementNode

using StmtNodePtr = std::shared_ptr<StatementNode>;

struct LoopStmtNode : public StatementNode
{
    LoopStmtNode(const Tokenizer& lexer, NodeKind kind)
        : StatementNode(lexer, kind) {}

    ExprNodePtr expr = nullptr;
    StmtNodePtr stmt = nullptr;

}; // LoopStmtNode

struct ExprStmtNode : public StatementNode
{
    ExprStmtNode(const Tokenizer& lexer, NodeKind kind)
        : StatementNode(lexer, kind) {}

    ExprNodePtr expr = nullptr;

}; // ExprStmtNode

struct LabelStmtNode : public StatementNode
{
    LabelStmtNode(const Tokenizer& lexer, NodeKind kind)
        : StatementNode(lexer, kind) {}

    char* id = nullptr;
    StmtNodePtr stmt = nullptr;
//    Label label;

}; // LabelStmtNode

struct CaseStmtNode : public StatementNode
{
    CaseStmtNode(const Tokenizer& lexer, NodeKind kind)
        : StatementNode(lexer, kind) {}

    ExprNodePtr expr = nullptr;
    StmtNodePtr stmt = nullptr;
    CaseStmtNode *nextCase;
//    BBlock respBB;

}; // CaseStmtNode

struct DefaultStmtNode : public StatementNode
{
    DefaultStmtNode(const Tokenizer& lexer, NodeKind kind)
        : StatementNode(lexer, kind) {}

    StmtNodePtr stmt = nullptr;
//    BBlock respBB;

}; // DefaultStmtNode

struct IfStmtNode : public StatementNode
{
    IfStmtNode(const Tokenizer& lexer, NodeKind kind)
        : StatementNode(lexer, kind) {}

    ExprNodePtr expr = nullptr;
    StmtNodePtr then_stmt = nullptr;
    StmtNodePtr else_stmt = nullptr;

}; // IfStmtNode

struct SwitchBucket
{
    int ncase;
    int	minVal;
    int maxVal;
    std::shared_ptr<CaseStmtNode> cases = nullptr;
    std::shared_ptr<CaseStmtNode> *tail = nullptr;

}; // SwitchBucket

struct SwitchStmtNode : public StatementNode
{
    SwitchStmtNode(const Tokenizer& lexer, NodeKind kind)
        : StatementNode(lexer, kind) {}

    ExprNodePtr expr = nullptr;
    StmtNodePtr stmt = nullptr;
    std::shared_ptr<CaseStmtNode>    cases = nullptr;
    std::shared_ptr<DefaultStmtNode> def_stmt = nullptr;
    SwitchBucket buckets;
    int nbucket;
    //BBlock nextBB;
    //BBlock defBB;

}; // SwitchStmtNode

struct DeclarationNode;
struct ForStmtNode : public StatementNode
{
    ForStmtNode(const Tokenizer& lexer, NodeKind kind)
        : StatementNode(lexer, kind) {}

    ExprNodePtr expr = nullptr;
    StmtNodePtr stmt = nullptr;
    std::shared_ptr<DeclarationNode> init_decl = nullptr;

    ExprNodePtr incr_expr = nullptr;
//    BBlock testBB;

}; // ForStmtNode

struct GotoStmtNode : public StatementNode
{
    GotoStmtNode(const Tokenizer& lexer, NodeKind kind)
        : StatementNode(lexer, kind) {}

    char *id = nullptr;
//    Label label;

}; // GotoStmtNode

struct BreakStmtNode : public StatementNode
{
    BreakStmtNode(const Tokenizer& lexer, NodeKind kind)
        : StatementNode(lexer, kind) {}

    StmtNodePtr target = nullptr;

}; // BreakStmtNode

struct ContinueStmtNode : public StatementNode
{
    ContinueStmtNode(const Tokenizer& lexer, NodeKind kind)
        : StatementNode(lexer, kind) {}

    std::shared_ptr<LoopStmtNode> target = nullptr;

}; // ContinueStmtNode

struct ReturnStmtNode : public StatementNode
{
    ReturnStmtNode(const Tokenizer& lexer, NodeKind kind)
        : StatementNode(lexer, kind) {}

    ExprNodePtr expr = nullptr;

}; // ReturnStmtNode

struct CompoundStmtNode : public StatementNode
{
    CompoundStmtNode(const Tokenizer& lexer, NodeKind kind)
        : StatementNode(lexer, kind) {}

#ifdef FEATURE_DECL_BEFORE_STAT
    NodePtr decls = nullptr;
#endif // FEATURE_DECL_BEFORE_STAT
    NodePtr stmts = nullptr;
//    Vector ilocals;

}; // CompoundStmtNode

class StatementParser
{
public:
    static StmtNodePtr ParseCompoundStatement(Parser& parser);

private:
    static StmtNodePtr ParseExpressionStatement(Parser& parser);
    static StmtNodePtr ParseLabelStatement(Parser& parser);
    static StmtNodePtr ParseCaseStatement(Parser& parser);
    static StmtNodePtr ParseDefaultStatement(Parser& parser);
    static StmtNodePtr ParseIfStatement(Parser& parser);
    static StmtNodePtr ParseSwitchStatement(Parser& parser);
    static StmtNodePtr ParseWhileStatement(Parser& parser);
    static StmtNodePtr ParseDoStatement(Parser& parser);
    static StmtNodePtr ParseForStatement(Parser& parser);
    static StmtNodePtr ParseGotoStatement(Parser& parser);
    static StmtNodePtr ParseBreakStatement(Parser& parser);
    static StmtNodePtr ParseContinueStatement(Parser& parser);
    static StmtNodePtr ParseReturnStatement(Parser& parser);
    static StmtNodePtr ParseStatement(Parser& parser);

}; // StatementParser

}
}