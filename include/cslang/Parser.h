#pragma once

#include "cslang/Tokenizer.h"
#include "cslang/Declaration.h"

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
    auto& CurrToken() const { return m_curr_token; }

    lexer::TokenTemplate<TokenType> PeekToken();

    void SkipTo(int toks[]);

    void Expect(const TokenType mask);

    void IncreaseLevle() { ++m_level; }
    void DecreaseLevle() { --m_level; }
    bool IsTypedefName(const std::string& id) const;
    bool IsTypeName(const lexer::TokenTemplate<TokenType>& token) const;
    void CheckTypedefName(int sclass, const std::string& id);
    void PostCheckTypedef();

private:
    virtual std::map<TokenType, std::string> TokenNames() const {
        return std::map<TokenType, std::string>();
    }

private:
    typedef Tokenizer::Token Token;

    cslang::Tokenizer m_tokenizer;

    lexer::TokenTemplate<TokenType> m_curr_token;

    // symbol
    int m_level = 0;
    std::vector<ast::TDName> m_typedef_names, m_overload_names;

}; // Parser

}