#pragma once

#include "cslang/Tokenizer.h"

#include <lexer/Parser.h>

namespace cslang
{

class Parser : public lexer::Parser<TokenType>
{
public:
    Parser(const char* str);

//    void Parse();

    auto& GetTokenizer() const { return m_tokenizer; }

    void NextToken();
    auto CurrTokenType() const { return m_curr_token.GetType(); }

    lexer::TokenTemplate<TokenType> PeekToken();

    void SkipTo(int toks[]);

    void Expect(const TokenType mask);

    bool IsTypedefName(const std::string& id) const;

private:
    virtual std::map<TokenType, std::string> TokenNames() const {
        return std::map<TokenType, std::string>();
    }

private:
    typedef Tokenizer::Token Token;

    cslang::Tokenizer m_tokenizer;

    lexer::TokenTemplate<TokenType> m_curr_token;

    std::set<std::string> m_typedef_names;

}; // Parser

}