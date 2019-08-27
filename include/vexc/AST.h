#pragma once

#include <memory>

namespace vexc
{

class Tokenizer;

enum NodeKind
{
	NK_TranslationUnit,     NK_Function,           NK_Declaration,
	NK_TypeName,            NK_Specifiers,         NK_Token,
	NK_TypedefName,         NK_EnumSpecifier,      NK_Enumerator,
	NK_StructSpecifier,     NK_UnionSpecifier,     NK_StructDeclaration,
	NK_StructDeclarator,    NK_PointerDeclarator,  NK_ArrayDeclarator,
	NK_FunctionDeclarator,  NK_ParameterTypeList,  NK_ParameterDeclaration,
	NK_NameDeclarator,      NK_InitDeclarator,     NK_Initializer,

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

    NodeKind kind = NK_MaxCount;

    Coord coord;

    std::shared_ptr<Node> next = nullptr;

}; // Node

using NodePtr = std::shared_ptr<Node>;

class ASTHelper
{
public:
    static bool CurrentTokenIn(int curr_token, int toks[]);
    static void PostCheckTypedef();

}; // ASTHelper

extern int FIRST_Declaration[];

}