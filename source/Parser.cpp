#include "vexc/Parser.h"
#include "vexc/AST.h"
#include "vexc/Type.h"

namespace vexc
{

Parser::Parser(const char* str)
    : m_tokenizer(str)
{
    m_curr_token = m_tokenizer.NextToken();
}

//void Parser::Parse()
//{
////    TokenType token;
//
//}

void Parser::NextToken()
{
    m_curr_token = m_tokenizer.NextToken();
}

lexer::TokenTemplate<TokenType>
Parser::PeekToken()
{
    return m_tokenizer.PeekToken();
}

void Parser::SkipTo(int toks[])
{
	int *p = toks;
//	Coord cord(m_tokenizer);

    if (ASTHelper::CurrentTokenIn(CurrTokenType(), toks) ||
        m_curr_token.GetType() == TK_END) {
        return;
    }

//	cord = TokenCoord;
	while (CurrTokenType() != TK_END)
	{
		p = toks;
		while (*p)
		{
            if (CurrTokenType() == *p) {
                return;
            }
			p++;
		}
		NextToken();
	}
}

void Parser::Expect(const TokenType mask)
{
    lexer::Parser<TokenType>::Expect(mask, m_curr_token);
}

}