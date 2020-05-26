#include "cslang/Parser.h"
#include "cslang/AST.h"
#include "cslang/Type.h"

namespace cslang
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
    if (m_curr_token.GetType() != mask) {
        throw lexer::ParserException(m_curr_token.Line(), m_curr_token.Column(),
            ExpectString(TokenName(mask), m_curr_token));
    }
}

bool Parser::IsTypedefName(const std::string& id) const
{
    for (auto& name : m_typedef_names) {
        if (name.id == id && name.level <= m_level && !name.overload) {
            return true;
        }
    }
    return false;
}

bool Parser::IsTypeName(const lexer::TokenTemplate<TokenType>& token) const
{
    auto type = token.GetType();
    std::string name(token.Begin(), token.End() - token.Begin());
    return type == TK_ID ? IsTypedefName(name) : (type >= TK_AUTO && type <= TK_VOID);
}

void Parser::CheckTypedefName(int sclass, const std::string& id)
{
    if (id.empty()) {
        return;
    }

	if (sclass == TK_TYPEDEF)
	{
        for (auto& name : m_typedef_names)
        {
            if (name.id == id)
            {
                if (m_level < name.level) {
                    name.level = m_level;
                }
                break;
            }
        }

        ast::TDName tn;
        tn.id       = id;
        tn.level    = m_level;
        tn.overload = 0;
        m_typedef_names.push_back(tn);
	}
	else
	{
        for (auto& name : m_typedef_names)
        {
            if (name.id == id && m_level > name.level) {
                name.overload = 1;
                m_overload_names.push_back(name);
                break;
            }
        }
	}
}

void Parser::PostCheckTypedef()
{
    m_overload_names.clear();
}

}