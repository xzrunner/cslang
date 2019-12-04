#pragma once

#include "vexc/AST.h"
#include "vexc/Type.h"
#include "vexc/Symbol.h"
#include "vexc/Expression.h"

#include <vector>

namespace vexc
{

class Parser;

namespace ast
{

enum { DEC_ABSTRACT = 0x01, DEC_CONCRETE = 0x02};
enum { POINTER_TO, ARRAY_OF, FUNCTION_RETURN };

struct DeclaratorNode;
struct StructDeclaratorNode : public Node
{
    StructDeclaratorNode(const Tokenizer& lexer, NodeKind kind)
        : Node(lexer, kind)
    {
    }

    std::shared_ptr<DeclaratorNode> dec = nullptr;
    ExprNodePtr expr = nullptr;

}; // StructDeclaratorNode

using StructDeclaratorNodePtr = std::shared_ptr<StructDeclaratorNode>;

struct SpecifiersNode;
struct StructDeclarationNode : public Node
{
    StructDeclarationNode(const Tokenizer& lexer, NodeKind kind)
        : Node(lexer, kind)
    {
    }

    std::shared_ptr<SpecifiersNode> specs = nullptr;
    NodePtr stDecs = nullptr;

}; // StructDeclarationNode

using StructDeclarationNodePtr = std::shared_ptr<StructDeclarationNode>;

struct StructSpecifierNode : public Node
{
    StructSpecifierNode(const Tokenizer& lexer, NodeKind kind)
        : Node(lexer, kind)
    {
    }

	char *id = nullptr;
    NodePtr stDecls = nullptr;
};

using StructSpecifierNodePtr = std::shared_ptr<StructSpecifierNode>;

struct EnumeratorNode : public Node
{
    EnumeratorNode(const Tokenizer& lexer, NodeKind kind)
        : Node(lexer, kind)
    {
    }

    char *id = nullptr;
    ExprNodePtr expr = nullptr;
};

using EnumeratorNodePtr = std::shared_ptr<EnumeratorNode>;

struct EnumSpecifierNode : public Node
{
    EnumSpecifierNode(const Tokenizer& lexer, NodeKind kind)
        : Node(lexer, kind)
    {
    }

    char* id = nullptr;
    NodePtr enumers = nullptr;
};

using EnumSpecifierNodePtr = std::shared_ptr<EnumSpecifierNode>;

struct TypedefNameNode : public Node
{
    TypedefNameNode(const Tokenizer& lexer, NodeKind kind)
        : Node(lexer, kind)
    {
    }

	char *id = nullptr;
	Symbol sym;
};

using TypedefNameNodePtr = std::shared_ptr<TypedefNameNode>;

struct TokenNode : public Node
{
    TokenNode(const Tokenizer& lexer, NodeKind kind)
        : Node(lexer, kind)
    {
    }

    int token = 0;

}; // TokenNode

using TokenNodePtr = std::shared_ptr<TokenNode>;

struct SpecifiersNode : public Node
{
    SpecifiersNode(const Tokenizer& lexer, NodeKind kind)
        : Node(lexer, kind)
    {
    }

    NodePtr stgClasses = nullptr;
    NodePtr tyQuals = nullptr;
    NodePtr tySpecs = nullptr;
    int sclass = 0;
    Type ty;

}; // SpecifiersNode

using SpecifiersNodePtr = std::shared_ptr<SpecifiersNode>;

struct TypeDerivList
{
	int ctor;
	union
	{
		int len;
		int qual;
		Signature sig;
	};

    TypeDerivList* next = nullptr;
};

struct DeclaratorNode : public Node
{
    DeclaratorNode(const Tokenizer& lexer, NodeKind kind)
        : Node(lexer, kind)
    {
    }

    std::shared_ptr<DeclaratorNode> dec = nullptr;
    char *id = nullptr;
    TypeDerivList tyDrvList;

}; // DeclaratorNode

using DeclaratorNodePtr = std::shared_ptr<DeclaratorNode>;

struct ParameterDeclarationNode : public Node
{
    ParameterDeclarationNode(const Tokenizer& lexer, NodeKind kind)
        : Node(lexer, kind)
    {
    }

    SpecifiersNodePtr specs = nullptr;
    DeclaratorNodePtr dec = nullptr;

}; // ParameterDeclarationNode

using ParameterDeclarationNodePtr = std::shared_ptr<ParameterDeclarationNode>;

struct ParameterTypeListNode : public Node
{
    ParameterTypeListNode(const Tokenizer& lexer, NodeKind kind)
        : Node(lexer, kind)
    {
    }

    NodePtr paramDecls = nullptr;
    int ellipse = 0;

}; // ParameterTypeListNode

using ParameterTypeListNodePtr = std::shared_ptr<ParameterTypeListNode>;

struct FunctionDeclaratorNode : public DeclaratorNode
{
    FunctionDeclaratorNode(const Tokenizer& lexer, NodeKind kind)
        : DeclaratorNode(lexer, kind)
    {
    }

    std::vector<std::string> ids;
    ParameterTypeListNodePtr paramTyList = nullptr;
    int partOfDef = 0;
    Signature sig;

}; // FunctionDeclaratorNode

using FunctionDeclaratorNodePtr = std::shared_ptr<FunctionDeclaratorNode>;

struct ArrayDeclaratorNode : public DeclaratorNode
{
    ArrayDeclaratorNode(const Tokenizer& lexer, NodeKind kind)
        : DeclaratorNode(lexer, kind)
    {
    }

    ExprNodePtr expr = nullptr;

}; // ArrayDeclaratorNode

using ArrayDeclaratorNodePtr = std::shared_ptr<ArrayDeclaratorNode>;

struct PointerDeclaratorNode : public DeclaratorNode
{
    PointerDeclaratorNode(const Tokenizer& lexer, NodeKind kind)
        : DeclaratorNode(lexer, kind)
    {
    }

    NodePtr tyQuals = nullptr;

}; // PointerDeclaratorNode

using PointerDeclaratorNodePtr = std::shared_ptr<PointerDeclaratorNode>;

struct TypeNameNode : public Node
{
    TypeNameNode(const Tokenizer& lexer, NodeKind kind)
        : Node(lexer, kind)
    {
    }

    SpecifiersNodePtr specs = nullptr;
    DeclaratorNodePtr dec = nullptr;

}; // TypeNameNode

using TypeNameNodePtr = std::shared_ptr<TypeNameNode>;

struct TDName
{
	char* id       = nullptr;
	int   level    = 0;
	int   overload = 0;
}; // TDName

class DeclarationParser
{
public:
    static TypeNameNodePtr ParseTypeName(Parser& parser);

    //AstDeclaration     ParseDeclaration(void);

    static bool IsTypeName(int tok);

private:
    static DeclaratorNodePtr ParseDeclarator(Parser& parser, int kind);
    static SpecifiersNodePtr ParseDeclarationSpecifiers(Parser& parser);
    static StructSpecifierNodePtr ParseStructOrUnionSpecifier(Parser& parser);
    static EnumeratorNodePtr ParseEnumerator(Parser& parser);
    static EnumSpecifierNodePtr ParseEnumSpecifier(Parser& parser);
    static DeclaratorNodePtr ParsePostfixDeclarator(Parser& parser, int kind);
    static DeclaratorNodePtr ParseDirectDeclarator(Parser& parser, int kind);
    static StructDeclarationNodePtr ParseStructDeclaration(Parser& parser);
    static StructDeclaratorNodePtr ParseStructDeclarator(Parser& parser);
    static ParameterTypeListNodePtr ParseParameterTypeList(Parser& parser);
    static ParameterDeclarationNodePtr ParseParameterDeclaration(Parser& parser);

}; // DeclarationParser

}
}