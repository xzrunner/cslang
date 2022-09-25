#pragma once

#include "cslang/predef.h"

#include <memory>

namespace cslang
{

class Tokenizer;
struct ExprNode;

enum NodeKind
{
	NK_TranslationUnit,     NK_Function,           NK_Declaration,
	NK_TypeName,            NK_Specifiers,         NK_Token,
	NK_TypedefName,         NK_EnumSpecifier,      NK_Enumerator,
	NK_StructSpecifier,     NK_UnionSpecifier,     NK_StructDeclaration,
	NK_StructDeclarator,    NK_PointerDeclarator,  NK_ArrayDeclarator,
	NK_FunctionDeclarator,  NK_ParameterTypeList,  NK_ParameterDeclaration,
	NK_NameDeclarator,      NK_InitDeclarator,     NK_Initializer,

#ifdef LANG_GLSL
    NK_UniformSpecifier,
#endif // LANG_GLSL

	NK_Expression,

	NK_ExpressionStatement, NK_LabelStatement,     NK_CaseStatement,
	NK_DefaultStatement,    NK_IfStatement,        NK_SwitchStatement,
	NK_WhileStatement,      NK_DoStatement,        NK_ForStatement,
	NK_GotoStatement,       NK_BreakStatement,     NK_ContinueStatement,
	NK_ReturnStatement,     NK_CompoundStatement,

    NK_MaxCount,
};

struct Coord
{
    Coord(const Tokenizer& lexer);

    size_t line   = 0;
    size_t column = 0;
};

struct Node
{
    Node(const Tokenizer& lexer, NodeKind kind);
    Node(const Coord& coord, NodeKind kind);
    virtual ~Node() {}

    NodeKind kind = NK_MaxCount;

    Coord coord;

    std::shared_ptr<Node> next = nullptr;

}; // Node

using NodePtr = std::shared_ptr<Node>;

struct Label
{
    Label(const Tokenizer& lexer) : coord(lexer) {}

    Coord coord;
    char *id;
    int ref;
    int defined;
    //BBlock respBB;
    Label *next;
};

struct InitData
{
    int offset;
    std::shared_ptr<ExprNode> expr = nullptr;
    std::shared_ptr<InitData> next = nullptr;
};

class ASTHelper
{
public:
    static bool CurrentTokenIn(int curr_token, int toks[]);
    static void PostCheckTypedef();

}; // ASTHelper

extern int FIRST_Declaration[];

}