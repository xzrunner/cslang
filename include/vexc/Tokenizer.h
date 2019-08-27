#pragma once

#include "vexc/Token.h"
#include "vexc/StringPool.h"

#include <lexer/Tokenizer.h>

#include <functional>
#include <array>

namespace vexc
{

union Value
{
	int    i[2];
	float  f;
	double d;
	void*  p;
};

class Tokenizer : public lexer::Tokenizer<unsigned int>
{
public:
    Tokenizer(const char* str);

    virtual Token PeekToken() override;

    void SetSkipEol(bool skip_eol);

    auto& GetTokenVal() const { return m_token_val; }

protected:
    virtual Token EmitToken() override;

private:
    void InitScanners();

    TokenType ScanIdentifier();
    TokenType ScanNumericLiteral();
    TokenType ScanBadChar();
    TokenType ScanEOF();
    TokenType ScanCharLiteral();
    TokenType ScanStringLiteral();
    TokenType ScanPlus();
    TokenType ScanMinus();
    TokenType ScanStar();
    TokenType ScanSlash();
    TokenType ScanPercent();
    TokenType ScanLess();
    TokenType ScanGreat();
    TokenType ScanExclamation();
    TokenType ScanEqual();
    TokenType ScanBar();
    TokenType ScanAmpersand();
    TokenType ScanCaret();
    TokenType ScanDot();
    TokenType ScanLBRACE();
    TokenType ScanRBRACE();
    TokenType ScanLBRACKET();
    TokenType ScanRBRACKET();
    TokenType ScanLPAREN();
    TokenType ScanRPAREN();
    TokenType ScanCOMMA();
    TokenType ScanSEMICOLON();
    TokenType ScanCOMP();
    TokenType ScanQUESTION();
    TokenType ScanCOLON();

    int ScanEscapeChar(int wide);
    TokenType ScanFloatLiteral(const char *start);
    TokenType ScanIntLiteral(const char *start, int len, int base);

    void SkipWhiteSpace();

    int FindKeyword(const char* str, int len) const;

private:
    static const unsigned char END_OF_FILE = 255;

public:
    static char* TokenStrings[];

private:
    bool m_skip_eol;

    std::array<std::function<TokenType(void)>, END_OF_FILE + 1> m_scanners;

    Value m_token_val;

    StringPool m_str_pool;

}; // Tokenizer

}