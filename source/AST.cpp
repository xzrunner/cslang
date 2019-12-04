#include "vexc/AST.h"
#include "vexc/Tokenizer.h"
#include "vexc/Declaration.h"

namespace vexc
{

Coord::Coord(const Tokenizer& lexer)
    : line(lexer.Line())
    , column(lexer.Column())
{
}

Node::Node(const Tokenizer& lexer, NodeKind kind)
    : kind(kind)
    , coord(lexer)
{
}

Node::Node(const Coord& coord, NodeKind kind)
    : kind(kind)
    , coord(coord)
{
}

bool ASTHelper::CurrentTokenIn(int curr_token, int toks[])
{
    int *p = toks;

    while (*p)
    {
        if (curr_token == *p) {
            return true;
        }
        p++;
    }

    return false;
}

void ASTHelper::PostCheckTypedef()
{
 //   int len = OverloadNames->len;
 //   int i = 0;
 //   for (i = 0; i < len; i++)
 //   {
 //       auto tn = (TDName)OverloadNames->data[i];
 //       tn->overload = 0;
 //   }

	//OverloadNames->len = 0;
}

}