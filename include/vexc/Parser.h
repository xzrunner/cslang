#pragma once

#include "vexc/Tokenizer.h"

#include <lexer/Parser.h>

namespace vexc
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

    void ExpectCurrToken(const TokenType mask);

private:
    virtual std::map<TokenType, std::string> TokenNames() const {
        return std::map<TokenType, std::string>();
    }

private:
    typedef Tokenizer::Token Token;

    vexc::Tokenizer m_tokenizer;

    lexer::TokenTemplate<TokenType> m_curr_token;

}; // Parser

}