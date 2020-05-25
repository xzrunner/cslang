#include "cslang/Tokenizer.h"
#include "cslang/keyword.h"

#define HIGH_4BIT(v)       ((v) >> (8 * sizeof(int) - 4) & 0x0f)
#define HIGH_3BIT(v)       ((v) >> (8 * sizeof(int) - 3) & 0x07)
#define HIGH_1BIT(v)       ((v) >> (8 * sizeof(int) - 1) & 0x01)

namespace
{

// todo: wide
void AppendSTR(std::string& str, char* tmp, int len, int wide)
{
    for (int i = 0; i < len; ++i) {
        str.push_back(tmp[i]);
    }
    str.push_back('\0');
}

}

namespace cslang
{

char* Tokenizer::TokenStrings[] =
{
    "begin",
#define TOKEN(k, s) s,
#include "cslang/token_cfg.h"
#undef  TOKEN
};

Tokenizer::Tokenizer(const char* str)
    : lexer::Tokenizer<TokenType>(str, str + strlen(str), "\"", '\\')
{
    InitScanners();
}

lexer::Tokenizer<TokenType>::Token
Tokenizer::PeekToken()
{
    auto old_val = m_token_val;
    auto ret = lexer::Tokenizer<TokenType>::PeekToken();
    m_token_val = old_val;
    return ret;
}

void Tokenizer::SetSkipEol(bool skip_eol)
{
}

lexer::Tokenizer<TokenType>::Token
Tokenizer::EmitToken()
{
    DiscardWhile(Whitespace());

    size_t start_line = Line();
    size_t start_column = Column();
    if (Eof())
    {
        return Token(TK_END, nullptr, nullptr, Length(), start_line, start_column);
    }
    else
    {
        const char* begin_pos = CurPos();
        auto token = m_scanners[(const unsigned char)(*begin_pos)]();
        const char* end_pos = CurPos();
        return Token(token, begin_pos, end_pos, Offset(begin_pos), start_line, start_column);
    }
}

void Tokenizer::InitScanners()
{
    for (int i = 0, n = END_OF_FILE + 1; i < n; ++i)
    {
        if (IsLetter(i)) {
            m_scanners[i] = std::bind(&Tokenizer::ScanIdentifier, this);
        } else if (IsDigit(i)) {
            m_scanners[i] = std::bind(&Tokenizer::ScanNumericLiteral, this);
        } else {
            m_scanners[i] = std::bind(&Tokenizer::ScanBadChar, this);
        }
    }

    m_scanners[END_OF_FILE] = std::bind(&Tokenizer::ScanEOF, this);
    m_scanners['\''] = std::bind(&Tokenizer::ScanCharLiteral, this);
    m_scanners['"'] = std::bind(&Tokenizer::ScanStringLiteralDoubleQuotes, this);
    m_scanners['`'] = std::bind(&Tokenizer::ScanStringLiteralSingleQuotes, this);
    m_scanners['+'] = std::bind(&Tokenizer::ScanPlus, this);
    m_scanners['-'] = std::bind(&Tokenizer::ScanMinus, this);
    m_scanners['*'] = std::bind(&Tokenizer::ScanStar, this);
    m_scanners['/'] = std::bind(&Tokenizer::ScanSlash, this);
    m_scanners['%'] = std::bind(&Tokenizer::ScanPercent, this);
    m_scanners['<'] = std::bind(&Tokenizer::ScanLess, this);
    m_scanners['>'] = std::bind(&Tokenizer::ScanGreat, this);
    m_scanners['!'] = std::bind(&Tokenizer::ScanExclamation, this);
    m_scanners['='] = std::bind(&Tokenizer::ScanEqual, this);
    m_scanners['|'] = std::bind(&Tokenizer::ScanBar, this);
    m_scanners['&'] = std::bind(&Tokenizer::ScanAmpersand, this);
    m_scanners['^'] = std::bind(&Tokenizer::ScanCaret, this);
    m_scanners['.'] = std::bind(&Tokenizer::ScanDot, this);
    m_scanners['{'] = std::bind(&Tokenizer::ScanLBRACE, this);
    m_scanners['}'] = std::bind(&Tokenizer::ScanRBRACE, this);
    m_scanners['['] = std::bind(&Tokenizer::ScanLBRACKET, this);
    m_scanners[']'] = std::bind(&Tokenizer::ScanRBRACKET, this);
    m_scanners['('] = std::bind(&Tokenizer::ScanLPAREN, this);
    m_scanners[')'] = std::bind(&Tokenizer::ScanRPAREN, this);
    m_scanners[','] = std::bind(&Tokenizer::ScanCOMMA, this);
    m_scanners[';'] = std::bind(&Tokenizer::ScanSEMICOLON, this);
    m_scanners['~'] = std::bind(&Tokenizer::ScanCOMP, this);
    m_scanners['?'] = std::bind(&Tokenizer::ScanQUESTION, this);
    m_scanners[':'] = std::bind(&Tokenizer::ScanCOLON, this);
    m_scanners['@'] = std::bind(&Tokenizer::ScanIdentifier, this);
    m_scanners['$'] = std::bind(&Tokenizer::ScanIdentifier, this);
}

TokenType Tokenizer::ScanIdentifier()
{
	auto start = CurPos();
	int tok;

	if (CurChar() == 'L')
	{
		if (NextChar() == '\'')
		{
			return ScanCharLiteral();
		}
		if (NextChar() == '"')
		{
			return ScanStringLiteralDoubleQuotes();
		}
        if (NextChar() == '`')
        {
            return ScanStringLiteralSingleQuotes();
        }
	}

    Advance();
	while (CurChar() == '@' || IsLetterOrDigit(CurChar()))
	{
		Advance();
	}

    tok = FindKeyword(start, (int)(CurPos() - start));
	if (tok == TK_ID) {
        m_token_val.p = (void*)(m_str_pool.InsertAndQuery(start, (int)(CurPos() - start)));
	}

	return static_cast<TokenType>(tok);
}

TokenType Tokenizer::ScanNumericLiteral()
{
    auto start = CurPos();
	int base = 10;

	if (CurChar() == '.')
	{
		return ScanFloatLiteral(start);
	}

	if (CurChar() == '0' && (NextChar() == 'x' || NextChar() == 'X'))
	{
        Advance(2);
		start = CurPos();
		base = 16;
		if (!IsHexDigit(CurChar()))
		{
//			Error(&TokenCoord, "Expect hex digit");
            m_token_val.i[0] = 0;
			return TK_INTCONST;
		}
		while (IsHexDigit(CurChar()))
		{
			Advance();
		}
	}
	else if (CurChar() == '0')
	{
		Advance();
		base = 8;
		while (IsOctDigit(CurChar()))
		{
			Advance();
		}
	}
	else
	{
		Advance();
		while (IsDigit(CurChar()))
		{
			Advance();
		}
	}

	if (base == 16 || (CurChar() != '.' && CurChar() != 'e' && CurChar() != 'E'))
	{
		return ScanIntLiteral(start, (int)(CurPos() - start), base);
	}
	else
	{
		return ScanFloatLiteral(start);
	}
}

TokenType Tokenizer::ScanBadChar()
{
    assert(0);
//    Error(&TokenCoord, "illegal character:\\x%x", CurChar());
    return NextToken().GetType();
}

TokenType Tokenizer::ScanEOF()
{
    return cslang::TK_END;
}

TokenType Tokenizer::ScanCharLiteral()
{
	int ch = 0;
	int count = 0;
	int wide = 0;

	if (CurChar() == 'L')
	{
		Advance();
		wide = 1;
	}
	Advance();
	while (CurChar() != '\'')
	{
        if (CurChar() == '\n' || CurChar() == END_OF_FILE) {
            break;
        }

        ch = CurChar() == '\\' ? ScanEscapeChar(wide) : CurCharAndAdvance();
		count++;
	}

	if (CurChar() != '\'')
	{
//		Error(&TokenCoord, "Expect '");
	}
    else
    {
        Advance();
        if (count > 1)
        {
//            Warning(&TokenCoord, "Two many characters");
        }
    }

    m_token_val.i[0] = ch;
    m_token_val.i[1] = 0;

	return TK_INTCONST;
}

TokenType Tokenizer::ScanStringLiteral(const char separator)
{
	char tmp[512];
	char *cp = tmp;
	int *wcp = (int *)tmp;
	int wide = 0;
	int len = 0;
	int maxlen = 512;
	int ch;
	std::string str;

	if (CurChar() == 'L')
	{
        Advance();
		wide = 1;
		maxlen /= sizeof(int);
	}
    Advance();

next_string:
	while (CurChar() != separator)
	{
		if (CurChar() == '\n' || CurChar() == END_OF_FILE)
			break;

		ch = CurChar() == '\\' ? ScanEscapeChar(wide) : CurCharAndAdvance();
		if (wide)
		{
			wcp[len] = ch;
		}
		else
		{
			cp[len] = (char)ch;
		}
		len++;
		if (len >= maxlen)
		{
            AppendSTR(str, tmp, len, wide);
			len = 0;
		}
	}

	if (CurChar() != separator)
	{
//		Error(&TokenCoord, "Expect \"");
		goto end_string;
	}

	Advance();
	SkipWhiteSpace();
	if (CurChar() == separator)
	{
		if (wide == 1)
		{
//			Error(&TokenCoord, "String wideness mismatch");
		}
		Advance();
		goto next_string;
	}
	else if (CurChar() == 'L' && NextChar() == separator)
	{
		if (wide == 0)
		{
//			Error(&TokenCoord, "String wideness mismatch");
		}
        Advance(2);
		goto next_string;
	}

end_string:
    AppendSTR(str, tmp, len, wide);
    m_token_val.p = (void*)(m_str_pool.InsertAndQuery(str));

	return wide ? TK_WIDESTRING : TK_STRING;
}

TokenType Tokenizer::ScanStringLiteralDoubleQuotes()
{
    return ScanStringLiteral('"');
}

TokenType Tokenizer::ScanStringLiteralSingleQuotes()
{
    return ScanStringLiteral('`');
}

TokenType Tokenizer::ScanPlus()
{
    Advance();
    if (CurChar() == '+')
    {
        Advance();
        return TK_INC;
    }
    else if (CurChar() == '=')
    {
        Advance();
        return TK_ADD_ASSIGN;
    }
    else
    {
        return TK_ADD;
    }
}

TokenType Tokenizer::ScanMinus()
{
	Advance();
	if (CurChar() == '-')
	{
		Advance();
		return TK_DEC;
	}
	else if (CurChar() == '=')
	{
		Advance();
		return TK_SUB_ASSIGN;
	}
	else if (CurChar() == '>')
	{
		Advance();
		return TK_POINTER;
	}
	else
	{
		return TK_SUB;
	}
}

TokenType Tokenizer::ScanStar()
{
    Advance();
    if (CurChar() == '=')
    {
        Advance();
        return TK_MUL_ASSIGN;
    }
    else
    {
        return TK_MUL;
    }
}

TokenType Tokenizer::ScanSlash()
{
    Advance();
    if (CurChar() == '=')
    {
        Advance();
        return TK_DIV_ASSIGN;
    }
    else
    {
        return TK_DIV;
    }
}

TokenType Tokenizer::ScanPercent()
{
    Advance();
    if (CurChar() == '=')
    {
        Advance();
        return TK_MOD_ASSIGN;
    }
    else
    {
        return TK_MOD;
    }
}

TokenType Tokenizer::ScanLess()
{
	Advance();
	if (CurChar() == '<')
	{
		Advance();
		if (CurChar() == '=')
		{
			Advance();
			return TK_LSHIFT_ASSIGN;
		}
		return TK_LSHIFT;
	}
	else if (CurChar() == '=')
	{
		Advance();
		return TK_LESS_EQ;
	}
	else
	{
		return TK_LESS;
	}
}

TokenType Tokenizer::ScanGreat()
{
	Advance();
	if (CurChar() == '>')
	{
		Advance();
		if (CurChar() == '=')
		{
			Advance();
			return TK_RSHIFT_ASSIGN;
		}
		return TK_RSHIFT;
	}
	else if (CurChar() == '=')
	{
		Advance();
		return TK_GREAT_EQ;
	}
	else
	{
		return TK_GREAT;
	}
}

TokenType Tokenizer::ScanExclamation()
{
	Advance();
	if (CurChar() == '=')
	{
		Advance();
		return TK_UNEQUAL;
	}
	else
	{
		return TK_NOT;
	}
}

TokenType Tokenizer::ScanEqual()
{
    Advance();
    if (CurChar() == '=')
    {
        Advance();
        return TK_EQUAL;
    }
    else
    {
        return TK_ASSIGN;
    }
}

TokenType Tokenizer::ScanBar()
{
	Advance();
	if (CurChar() == '|')
	{
		Advance();
		return TK_OR;
	}
	else if (CurChar() == '=')
	{
		Advance();
		return TK_BITOR_ASSIGN;
	}
	else
	{
		return TK_BITOR;
	}
}

TokenType Tokenizer::ScanAmpersand()
{
    Advance();
    if (CurChar() == '&')
    {
        Advance();
        return TK_AND;
    }
    else if (CurChar() == '=')
    {
        Advance();
        return TK_BITAND_ASSIGN;
    }
    else
    {
        return TK_BITAND;
    }
}

TokenType Tokenizer::ScanCaret()
{
    Advance();
    if (CurChar() == '=')
    {
        Advance();
        return TK_BITXOR_ASSIGN;
    }
    else
    {
        return TK_BITXOR;
    }
}

TokenType Tokenizer::ScanDot()
{
	if (IsDigit(NextChar()))
	{
		return ScanFloatLiteral(CurPos());
	}
	else if (NextChar() == '.' && NextNextChar() == '.')
	{
        Advance(3);
		return TK_ELLIPSE;
	}
	else
	{
		Advance();
		return TK_DOT;
	}
}

#define SINGLE_CHAR_SCANNER(t) \
TokenType Tokenizer::Scan##t() \
{                              \
    Advance();                 \
    return TK_##t;             \
}

SINGLE_CHAR_SCANNER(LBRACE)
SINGLE_CHAR_SCANNER(RBRACE)
SINGLE_CHAR_SCANNER(LBRACKET)
SINGLE_CHAR_SCANNER(RBRACKET)
SINGLE_CHAR_SCANNER(LPAREN)
SINGLE_CHAR_SCANNER(RPAREN)
SINGLE_CHAR_SCANNER(COMMA)
SINGLE_CHAR_SCANNER(SEMICOLON)
SINGLE_CHAR_SCANNER(COMP)
SINGLE_CHAR_SCANNER(QUESTION)
SINGLE_CHAR_SCANNER(COLON)

int Tokenizer::ScanEscapeChar(int wide)
{
	int v = 0, overflow = 0;

    Advance();
    auto c = CurChar();
    Advance();
	switch (c)
	{
	case 'a':
		return '\a';

	case 'b':
		return '\b';

	case 'f':
		return '\f';

	case 'n':
		return '\n';

	case 'r':
		return '\r';

	case 't':
		return '\t';

	case 'v':
		return '\v';

	case '\'':
	case '"':
    case '`':
	case '\\':
	case '\?':
		return c;

	case 'x':
		if (!IsHexDigit(CurChar()))
		{
//			Error(&TokenCoord, "Expect hex digit");
			return 'x';
		}
		v = 0;
		while (IsHexDigit(CurChar()))
		{
			//if (v >> (WCharType->size - 4))
			//{
			//	overflow = 1;
			//}
			if (IsDigit(CurChar()))
			{
				v = (v << 4) + CurChar() - '0';
			}
			else
			{
				v = (v << 4) + ToUpper(CurChar()) - 'A' + 10;
			}
            Advance();
		}
		if (overflow || (! wide && v > 255))
		{
//			Warning(&TokenCoord, "Hexademical espace sequence overflow");
		}
		return v;

	case '0': case '1': case '2': case '3':
	case '4': case '5': case '6': case '7':
		v = c - '0';
		if (IsOctDigit(CurChar()))
		{
			v = (v << 3) + CurChar() - '0';
            Advance();
            if (IsOctDigit(CurChar())) {
                v = (v << 3) + CurChar() - '0';
                Advance();
            }
		}
		return v;

	default:
//		Warning(&TokenCoord, "Unrecognized escape sequence:\\%c", CurChar());
		return CurChar();
	}
}

TokenType Tokenizer::ScanFloatLiteral(const char* start)
{
	double d;

	if (CurChar() == '.')
	{
		Advance();
		while (IsDigit(CurChar()))
		{
			Advance();
		}
	}
	if (CurChar() == 'e' || CurChar() == 'E')
	{
		Advance();
		if (CurChar() == '+' || CurChar() == '-')
		{
			Advance();
		}
		if (! IsDigit(CurChar()))
		{
//			Error(&TokenCoord, "Expect exponent value");
		}
		else
		{
			while (IsDigit(CurChar()))
			{
				Advance();
			}
		}
	}

	errno = 0;
	d = strtod((char *)start, NULL);
	if (errno == ERANGE)
	{
//		Warning(&TokenCoord, "Float literal overflow");
	}
    m_token_val.d = d;
	if (CurChar() == 'f' || CurChar() == 'F')
	{
		Advance();
        m_token_val.f = (float)d;
		return TK_FLOATCONST;
	}
	else if (CurChar() == 'L' || CurChar() == 'l')
	{
		Advance();
		return TK_LDOUBLECONST;
	}
	else
	{
		return TK_DOUBLECONST;
	}
}

TokenType Tokenizer::ScanIntLiteral(const char* start, int len, int base)
{
	const char *p =  start;
    const char *end = start + len;
	unsigned int i[2] = {0, 0};
    TokenType tok = TK_INTCONST;
	int d = 0;
	int carry0 = 0, carry1 = 0;
	int overflow = 0;

	while (p != end)
	{
		if (base == 16)
		{
			if ((*p >= 'A' && *p <= 'F') ||
				(*p >= 'a' && *p <= 'f'))
			{
				d = ToUpper(*p) - 'A' + 10;
			}
			else
			{
				d = *p - '0';
			}
		}
		else
		{
			d = *p - '0';
		}

		switch (base)
		{
		case 16:
			carry0 = HIGH_4BIT(i[0]);
			carry1 = HIGH_4BIT(i[1]);
			i[0] = i[0] << 4;
			i[1] = i[1] << 4;
			break;

		case 8:
			carry0 = HIGH_3BIT(i[0]);
			carry1 = HIGH_3BIT(i[1]);
			i[0] = i[0] << 3;
			i[1] = i[1] << 3;
			break;

		case 10:
			{
				unsigned int t1, t2;

				carry0 = HIGH_3BIT(i[0]) + HIGH_1BIT(i[0]);
				carry1 = HIGH_3BIT(i[1]) + HIGH_1BIT(i[1]);
				t1 = i[0] << 3;
				t2 = i[0] << 1;
				if (t1 > UINT_MAX - t2)
				{
					carry0++;
				}
				i[0] = t1 + t2;
				t1 = i[1] << 3;
				t2 = i[1] << 1;
				if (t1 > UINT_MAX - t2)
				{
					carry1++;
				}
				i[1] = t1 + t2;
			}
			break;
		}
		if (i[0] > UINT_MAX - d)
		{
			carry0 += i[0] - (UINT_MAX - d);
		}
		if (carry1 || (i[1] > UINT_MAX - carry0))
		{
			overflow = 1;
		}
		i[0] += d;
		i[1] += carry0;
		p++;
	}

	if (overflow || i[1] != 0)
	{
//		Warning(&TokenCoord, "Integer literal is too big");
	}

    m_token_val.i[1] = 0;
    m_token_val.i[0] = i[0];
	tok = TK_INTCONST;

	if (CurChar() == 'U' || CurChar() == 'u')
	{
		Advance();
		if (tok == TK_INTCONST)
		{
			tok = TK_UINTCONST;
		}
		else if (tok == TK_LLONGCONST)
		{
			tok = TK_ULLONGCONST;
		}
	}

	if (CurChar() == 'L' || CurChar() == 'l')
	{
		Advance();
		if (tok == TK_INTCONST)
		{
			tok = TK_LONGCONST;
		}
		else if (tok == TK_UINTCONST)
		{
			tok = TK_ULONGCONST;
		}
		if (CurChar() == 'L' || CurChar() == 'l')
		{
			Advance();
			if (tok < TK_LLONGCONST)
			{
				tok = TK_LLONGCONST;
			}
		}
	}

	return tok;
}

void Tokenizer::SkipWhiteSpace()
{
	int ch;

//again:
	ch = CurChar();
	while (ch == '\t' || ch == '\v' || ch == '\f' || ch == ' ' ||
	       ch == '\r' || ch == '\n' || ch == '/'  || ch == '#')
	{
		switch (ch)
		{
		case '\n':
			//TokenCoord.ppline++;
			//LINE++;
			//LINEHEAD = ++CURSOR;
			break;

		case '#':
//			ScanPPLine();
			break;

		case '/':
            if (NextChar() != '/' && NextChar() != '*') {
                return;
            }
            Advance();
			if (CurChar() == '/')
			{
                Advance();
                while (CurChar() != '\n' && CurChar() != END_OF_FILE)
				{
					Advance();
				}
			}
			else
			{
                Advance(2);
				while (CurChar() != '*' || NextChar() != '/')
				{
					if (CurChar() == '\n')
					{
						//TokenCoord.ppline++;
						//LINE++;
					}
					else if (CurChar() == END_OF_FILE || NextChar() == END_OF_FILE)
					{
                        assert(0);
//						Error(&TokenCoord, "Comment is not closed");
						return;
					}
					Advance();
				}
                Advance(2);
			}
			break;

		default:
			Advance();
			break;
		}
		ch = CurChar();
	}

	//if (ExtraWhiteSpace != NULL)
	//{
	//	char *p;

	//	FOR_EACH_ITEM(char*, p, ExtraWhiteSpace)
	//		if (strncmp(CURSOR, p, strlen(p)) == 0)
	//		{
	//			CURSOR += strlen(p);
	//			goto again;
	//		}
	//	ENDFOR
	//}
}

int Tokenizer::FindKeyword(const char* str, int len) const
{
    struct keyword *p = NULL;
    int index = 0;

    if (IsLetter(*str) && *str != '_') {
        index = ToUpper(*str) - 'A' + 1;
    }

    p = keywords[index];
    while (p->name)
    {
        if (p->len == len && strncmp((const char*)(str), p->name, len) == 0) {
            break;
        }
        p++;
    }
    return p->tok;
}

}