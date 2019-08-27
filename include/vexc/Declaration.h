#pragma once

#include "vexc/AST.h"

namespace vexc
{
namespace ast
{

//struct SpecifiersNode : public Node
//{
//    NodePtr stgClasses = nullptr;
//    NodePtr tyQuals = nullptr;
//    NodePtr tySpecs = nullptr;
//    int sclass = 0;
//    Type ty;
//
//}; // SpecifiersNode
//
//struct DeclaratorNode : public Node
//{
//    struct astDeclarator *dec;
//    char *id;
//    TypeDerivList tyDrvList;
//
//}; // DeclaratorNode
//
//struct TypeNameNode : public Node
//{
//    TypeNameNode(const Tokenizer& lexer, NodeKind kind)
//        : Node(lexer, kind) {}
//
//    SpecifiersNode specs;
//    DeclaratorNode dec;
//
//}; // TypeNameNode

struct TDName
{
	char* id       = nullptr;
	int   level    = 0;
	int   overload = 0;
}; // TDName

class DeclarationParser
{
public:
    //AstTypeName        ParseTypeName(void);
    //AstDeclaration     ParseDeclaration(void);

    static bool IsTypeName(int tok);

private:
    static bool IsTypedefName(const char* id);

}; // DeclarationParser

}
}