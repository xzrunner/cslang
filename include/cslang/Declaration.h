#pragma once

#include "cslang/predef.h"
#include "cslang/AST.h"
#include "cslang/Type.h"
#include "cslang/Symbol.h"
#include "cslang/Expression.h"
#include "cslang/Statement.h"

#include <vector>

namespace cslang
{

class Parser;

namespace ast
{

enum { DEC_ABSTRACT = 0x01, DEC_CONCRETE = 0x02};
enum { POINTER_TO, ARRAY_OF, FUNCTION_RETURN };

struct TDName
{
    std::string id;
    int level    = 0;
    int overload = 0;
}; // TDName

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

struct InitializerNode : public Node
{
    InitializerNode(const Tokenizer& lexer, NodeKind kind)
        : Node(lexer, kind)
    {
    }

	int lbrace;
	//union
	//{
		NodePtr initials;
		ExprNodePtr expr;
	//};
	InitData idata;
};

using InitializerNodePtr = std::shared_ptr<InitializerNode>;

struct InitDeclaratorNode : public Node
{
    InitDeclaratorNode(const Tokenizer& lexer, NodeKind kind)
        : Node(lexer, kind)
    {
    }

	DeclaratorNodePtr dec;
	InitializerNodePtr init;
};

using InitDeclaratorNodePtr = std::shared_ptr<InitDeclaratorNode>;

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

struct DeclarationNode : public Node
{
    DeclarationNode(const Tokenizer& lexer, NodeKind kind)
        : Node(lexer, kind)
    {
    }

    SpecifiersNodePtr specs = nullptr;
    NodePtr initDecs = nullptr;
};

using DeclarationNodePtr = std::shared_ptr<DeclarationNode>;

struct FunctionNode : public Node
{
    FunctionNode(const Tokenizer& lexer, NodeKind kind)
        : Node(lexer, kind)
        , labels(lexer)
    {
    }

    SpecifiersNodePtr specs = nullptr;
    DeclaratorNodePtr dec = nullptr;
    FunctionDeclaratorNodePtr fdec = nullptr;
#ifdef FEATURE_DECL_BEFORE_STAT
    NodePtr decls = nullptr;
#endif // FEATURE_DECL_BEFORE_STAT
    StmtNodePtr stmt = nullptr;
    FunctionSymbol fsym;
    Label labels;
    //Vector loops;
    //Vector swtches;
    //Vector breakable;
    int hasReturn;
};

using FunctionNodePtr = std::shared_ptr<FunctionNode>;

struct TranslationUnitNode : public Node
{
    TranslationUnitNode(const Tokenizer& lexer, NodeKind kind)
        : Node(lexer, kind)
    {
    }

    NodePtr extDecls = nullptr;
};

using TranslationUnitNodePtr = std::shared_ptr<TranslationUnitNode>;

class DeclarationParser
{
public:
    static TypeNameNodePtr ParseTypeName(Parser& parser);

    static TranslationUnitNodePtr ParseTranslationUnit(Parser& parser);

    static DeclarationNodePtr ParseDeclaration(Parser& parser);

private:
    static DeclaratorNodePtr ParseDeclarator(Parser& parser, int kind);
    static SpecifiersNodePtr ParseDeclarationSpecifiers(Parser& parser);
    static StructSpecifierNodePtr ParseStructOrUnionSpecifier(Parser& parser);
    static EnumeratorNodePtr ParseEnumerator(Parser& parser);
    static EnumSpecifierNodePtr ParseEnumSpecifier(Parser& parser);
    static DeclaratorNodePtr ParsePostfixDeclarator(Parser& parser, int kind);
    static InitializerNodePtr ParseInitializer(Parser& parser);
    static InitDeclaratorNodePtr ParseInitDeclarator(Parser& parser);
    static DeclaratorNodePtr ParseDirectDeclarator(Parser& parser, int kind);
    static StructDeclarationNodePtr ParseStructDeclaration(Parser& parser);
    static StructDeclaratorNodePtr ParseStructDeclarator(Parser& parser);
    static ParameterTypeListNodePtr ParseParameterTypeList(Parser& parser);
    static ParameterDeclarationNodePtr ParseParameterDeclaration(Parser& parser);
    static DeclarationNodePtr ParseCommonHeader(Parser& parser);
    static FunctionDeclaratorNodePtr GetFunctionDeclarator(const InitDeclaratorNodePtr& initDec);
    static NodePtr ParseExternalDeclaration(Parser& parser);

}; // DeclarationParser

}
}