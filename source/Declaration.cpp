#include "vexc/Declaration.h"
#include "vexc/Token.h"
#include "vexc/grammer.h"
#include "vexc/Parser.h"
#include "vexc/Token.h"

namespace vexc
{

static int FIRST_StructDeclaration[] = { FIRST_DECLARATION, 0 };
static int FF_StructDeclaration[] = { FIRST_DECLARATION, TK_RBRACE, 0 };

int FIRST_Declaration[] = { FIRST_DECLARATION, 0 };

static bool CurrentTokenIn(int curr_token, int toks[])
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

namespace ast
{

/**
 * type-name:
 *     specifier-qualifier-list abstract-declarator
 */
TypeNameNodePtr DeclarationParser::ParseTypeName(Parser& parser)
{
    auto type_name = std::make_shared<TypeNameNode>(parser.GetTokenizer(), NK_Expression);

    type_name->specs = ParseDeclarationSpecifiers(parser);
	if (type_name->specs->stgClasses != NULL)
	{
        assert(0);
		//Error(&type_name->coord, "type name should not have storage class");
		type_name->specs->stgClasses = NULL;
	}
    type_name->dec = ParseDeclarator(parser, DEC_ABSTRACT);

	return type_name;
}

bool DeclarationParser::IsTypeName(int tok)
{
    return /*tok == TK_ID ? IsTypedefName((char*)(parser.GetTokenizer().GetTokenVal().p)) :*/ (tok >= TK_AUTO && tok <= TK_VOID);
}

/**
 *  abstract-declarator:
 *		pointer
 *		[pointer] direct-abstract-declarator
 *
 *  direct-abstract-declarator:
 *		( abstract-declarator )
 *		[direct-abstract-declarator] [ [constant-expression] ]
 *		[direct-abstract-declarator] ( [parameter-type-list] )
 *
 *  declarator:
 *		pointer declarator
 *		direct-declarator
 *
 *  direct-declarator:
 *		ID
 *		( declarator )
 *		direct-declarator [ [constant-expression] ]
 *		direct-declarator ( parameter-type-list )
 *		direct-declarator ( [identifier-list] )
 *
 *  pointer:
 *		* [type-qualifier-list]
 *		* [type-qualifier-list] pointer
 *
 *  We change the above standard grammar into more suitable form defined below.
 *  abstract-declarator:
 *		* [type-qualifer-list] abstract-declarator
 *		postfix-abstract-declarator
 *
 *  postfix-abstract-declarator:
 *		direct-abstract-declarator
 *		postfix-abstract-declarator [ [constant-expression] ]
 *		postfix-abstrace-declarator( [parameter-type-list] )
 *
 *  direct-abstract-declarator:
 *		( abstract-declarator )
 *		NULL
 *
 *  declarator:
 *		* [type-qualifier-list] declarator
 *		postfix-declarator
 *
 *  postfix-declarator:
 *		direct-declarator
 *		postfix-declarator [ [constant-expression] ]
 *		postfix-declarator ( parameter-type-list)
 *		postfix-declarator ( [identifier-list] )
 *
 *  direct-declarator:
 *		ID
 *		( declarator )
 *
 *	The declartor is similar as the abstract declarator, we use one function
 *	ParseDeclarator() to parse both of them. kind indicate to parse which kind
 *	of declarator. The possible value can be:
 *	DEC_CONCRETE: parse a declarator
 *	DEC_ABSTRACT: parse an abstract declarator
 *	DEC_CONCRETE | DEC_ABSTRACT: both of them are ok
 */
DeclaratorNodePtr DeclarationParser::ParseDeclarator(Parser& parser, int kind)
{
	if (parser.CurrTokenType() == TK_MUL)
	{
        auto ptrDec = std::make_shared<PointerDeclaratorNode>(parser.GetTokenizer(), NK_PointerDeclarator);

        auto prev = ptrDec->tyQuals;

        parser.NextToken();
        auto curr_token = parser.CurrTokenType();
		while (curr_token == TK_CONST || curr_token == TK_VOLATILE)
		{
            auto tok = std::make_shared<TokenNode>(parser.GetTokenizer(), NK_Token);
			tok->token = curr_token;
            prev->next = tok;
            prev = tok;
            parser.NextToken();
		}
		ptrDec->dec = ParseDeclarator(parser, kind);

		return ptrDec;
	}

	return ParsePostfixDeclarator(parser, kind);
}

/**
 *  declaration-specifiers:
 *		storage-class-specifier [declaration-specifiers]
 *		type-specifier [declaration-specifiers]
 *		type-qualifier [declaration-specifiers]
 *
 *  storage-class-specifier:
 *		auto
 *		register
 *		static
 *		extern
 *		typedef
 *
 *  type-qualifier:
 *		const
 *		volatile
 *
 *  type-specifier:
 *		void
 *		char
 *		short
 *		int
 *		long
 *		float
 *		double
 *		signed
 *		unsigned
 *		struct-or-union-specifier
 *		enum-specifier
 *		typedef-name
 */
SpecifiersNodePtr DeclarationParser::ParseDeclarationSpecifiers(Parser& parser)
{
    auto specs = std::make_shared<SpecifiersNode>(parser.GetTokenizer(), NK_Specifiers);

	auto scTail = &specs->stgClasses;
    auto tqTail = &specs->tyQuals;
    auto tsTail = &specs->tySpecs;

    int seeTy = 0;

next_specifier:
	switch (parser.CurrTokenType())
	{
	case TK_AUTO:
	case TK_REGISTER:
	case TK_STATIC:
	case TK_EXTERN:
	case TK_TYPEDEF:
    {
        auto tok = std::make_shared<TokenNode>(parser.GetTokenizer(), NK_Token);
        tok->token = parser.CurrTokenType();
        *scTail = tok;
        scTail = &tok->next;
        parser.NextToken();
    }
		break;

	case TK_CONST:
	case TK_VOLATILE:
    {
        auto tok = std::make_shared<TokenNode>(parser.GetTokenizer(), NK_Token);
        tok->token = parser.CurrTokenType();
        *tqTail = tok;
        tqTail = &tok->next;
        parser.NextToken();
    }
		break;

	case TK_VOID:
	case TK_CHAR:
	case TK_SHORT:
	case TK_INT:
	case TK_INT64:
	case TK_LONG:
	case TK_FLOAT:
	case TK_DOUBLE:
	case TK_SIGNED:
	case TK_UNSIGNED:
    {
        auto tok = std::make_shared<TokenNode>(parser.GetTokenizer(), NK_Token);
        tok->token = parser.CurrTokenType();
        *tsTail = tok;
        tsTail = &tok->next;
        seeTy = 1;
        parser.NextToken();
    }
		break;

	case TK_ID:
		if (! seeTy && parser.IsTypedefName((char*)(parser.GetTokenizer().GetTokenVal().p)))
		{
            auto tname = std::make_shared<TypedefNameNode>(parser.GetTokenizer(), NK_TypedefName);
			tname->id = (char*)(parser.GetTokenizer().GetTokenVal().p);
			*tsTail = tname;
			tsTail = &tname->next;
            parser.NextToken();
			seeTy = 1;
			break;
		}
		return specs;

	case TK_STRUCT:
	case TK_UNION:

		*tsTail = ParseStructOrUnionSpecifier(parser);
		tsTail = &(*tsTail)->next;
		seeTy = 1;
		break;

	case TK_ENUM:

		*tsTail = ParseEnumSpecifier(parser);
		tsTail = &(*tsTail)->next;
		seeTy = 1;
		break;

	default:
		return specs;
	}
	goto next_specifier;
}

/**
 *  struct-or-union-specifier:
 *		struct-or-union [identifier] { struct-declaration-list }
 *		struct-or-union identifier
 *
 *  struct-or-union:
 *		struct
 *		union
 *
 *  struct-declaration-list:
 *      struct-declaration
 *		struct-declaration-list struct-declaration
 */
StructSpecifierNodePtr DeclarationParser::ParseStructOrUnionSpecifier(Parser& parser)
{
	auto stSpec = std::make_shared<StructSpecifierNode>(parser.GetTokenizer(), NK_StructSpecifier);
	if (parser.CurrTokenType() == TK_UNION)
	{
		stSpec->kind = NK_UnionSpecifier;
	}

    NodePtr prev = nullptr;

	parser.NextToken();
	switch (parser.CurrTokenType())
	{
	case TK_ID:
		stSpec->id = (char*)(parser.GetTokenizer().GetTokenVal().p);
		parser.NextToken();
		if (parser.CurrTokenType() == TK_LBRACE)
			goto lbrace;

		return stSpec;

	case TK_LBRACE:
lbrace:
		parser.NextToken();
		if (parser.CurrTokenType() == TK_RBRACE)
		{
			parser.NextToken();
			return stSpec;
		}

		// FIXME: use a better way to handle errors in struct declaration list
        prev = stSpec;
		while (CurrentTokenIn(parser.CurrTokenType(), FIRST_StructDeclaration))
		{
			auto node = ParseStructDeclaration(parser);
            prev->next = node;
            prev = node;
			//SkipTo(FF_StructDeclaration, "the start of struct declaration or }");
            parser.SkipTo(FF_StructDeclaration);
		}
        parser.Expect(TK_RBRACE);
        parser.NextToken();
		return stSpec;

	default:
        assert(0);
		//Error(&TokenCoord, "Expect identifier or { after struct/union");
		return stSpec;
	}
}

/**
 *  enumerator:
 *		enumeration-constant
 *		enumeration-constant = constant-expression
 *
 *  enumeration-constant:
 *		identifier
 */
EnumeratorNodePtr DeclarationParser::ParseEnumerator(Parser& parser)
{
    auto enumer = std::make_shared<EnumeratorNode>(parser.GetTokenizer(), NK_Enumerator);

	if (parser.CurrTokenType() != TK_ID)
	{
        assert(0);
		//Error(&TokenCoord, "The eumeration constant must be identifier");
		return enumer;
	}

	enumer->id = (char*)(parser.GetTokenizer().GetTokenVal().p);
	parser.NextToken();
	if (parser.CurrTokenType() == TK_ASSIGN)
	{
		parser.NextToken();
		enumer->expr = ExpressionParser::ParseConstantExpression(parser);
	}

	return enumer;
}

/**
*  enum-specifier
*		enum [identifier] { enumerator-list }
*		enum [identifier] { enumerator-list , }
*		enum identifier
*
*  enumerator-list:
*		enumerator
*		enumerator-list , enumerator
*/
EnumSpecifierNodePtr DeclarationParser::ParseEnumSpecifier(Parser& parser)
{
	auto enumSpec = std::make_shared<EnumSpecifierNode>(parser.GetTokenizer(), NK_EnumSpecifier);

	parser.NextToken();
	if (parser.CurrTokenType() == TK_ID)
	{
		enumSpec->id = (char*)(parser.GetTokenizer().GetTokenVal().p);
		parser.NextToken();
		if (parser.CurrTokenType() == TK_LBRACE)
			goto enumerator_list;
	}
	else if (parser.CurrTokenType() == TK_LBRACE)
	{
enumerator_list:
		parser.NextToken();
		if (parser.CurrTokenType() == TK_RBRACE)
			return enumSpec;

		enumSpec->enumers = ParseEnumerator(parser);
		auto prev = enumSpec->enumers;
		while (parser.CurrTokenType() == TK_COMMA)
		{
			parser.NextToken();
			if (parser.CurrTokenType() == TK_RBRACE)
				break;
			auto node = ParseEnumerator(parser);
            prev->next = node;
            prev = node;
		}
		parser.Expect(TK_RBRACE);
        parser.NextToken();
	}
	else
	{
        assert(0);
		//Error(&TokenCoord, "Expect identifier or { after enum");
	}

	return enumSpec;
}

/**
 *  postfix-declarator:
 *		direct-declarator
 *		postfix-declarator [ [constant-expression] ]
 *		postfix-declarator ( parameter-type-list)
 *		postfix-declarator ( [identifier-list] )
 *
 *  postfix-abstract-declarator:
 *		direct-abstract-declarator
 *		postfix-abstract-declarator ( [parameter-type-list] )
 *		postfix-abstract-declarator [ [constant-expression] ]
 */
DeclaratorNodePtr DeclarationParser::ParsePostfixDeclarator(Parser& parser, int kind)
{
	auto dec = ParseDirectDeclarator(parser, kind);

	while (1)
	{
		if (parser.CurrTokenType() == TK_LBRACKET)
		{
            auto arrDec = std::make_shared<ArrayDeclaratorNode>(parser.GetTokenizer(), NK_ArrayDeclarator);
			arrDec->dec = dec;

			parser.NextToken();
			if (parser.CurrTokenType() != TK_RBRACKET)
			{
				arrDec->expr = ExpressionParser::ParseConstantExpression(parser);
			}
			parser.Expect(TK_RBRACKET);
            parser.NextToken();

			dec = arrDec;
		}
		else if (parser.CurrTokenType() == TK_LPAREN)
		{
			/// notice: for abstract declarator, identifier list is not permitted,
			/// but we leave this to semantic check
			auto funcDec = std::make_shared<FunctionDeclaratorNode>(parser.GetTokenizer(), NK_FunctionDeclarator);
			funcDec->dec = dec;

			parser.NextToken();
			if (IsTypeName(parser.CurrTokenType()))
			{
				funcDec->paramTyList = ParseParameterTypeList(parser);
			}
			else
			{
				if (parser.CurrTokenType() == TK_ID)
				{
                    funcDec->ids.push_back((char*)(parser.GetTokenizer().GetTokenVal().p));
					parser.NextToken();
					while (parser.CurrTokenType() == TK_COMMA)
					{
						parser.NextToken();
						if (parser.CurrTokenType() == TK_ID) {
                            funcDec->ids.push_back((char*)(parser.GetTokenizer().GetTokenVal().p));
						}
                        parser.Expect(TK_ID);
                        parser.NextToken();
					}
				}
			}
			parser.Expect(TK_RPAREN);
            parser.NextToken();
			dec = funcDec;
		}
		else
		{
			return dec;
		}
	}
}

/**
 *  direct-declarator:
 *		ID
 *		( declarator )
 *
 *  direct-abstract-declarator:
 *		( abstract-declarator)
 *		nil
 */
DeclaratorNodePtr DeclarationParser::ParseDirectDeclarator(Parser& parser, int kind)
{
	if (parser.CurrTokenType() == TK_LPAREN)
	{
		parser.NextToken();
		auto dec = ParseDeclarator(parser, kind);
		parser.Expect(TK_RPAREN);
        parser.NextToken();

		return dec;
	}

    auto dec = std::make_shared<DeclaratorNode>(parser.GetTokenizer(), NK_NameDeclarator);
	if (parser.CurrTokenType() == TK_ID)
	{
		if (kind == DEC_ABSTRACT)
		{
            assert(0);
			//Error(&TokenCoord, "Identifier is not permitted in the abstract declarator");
		}

		dec->id = (char*)(parser.GetTokenizer().GetTokenVal().p);
		parser.NextToken();
	}
	else if (kind == DEC_CONCRETE)
	{
        assert(0);
		//Error(&TokenCoord, "Expect identifier");
	}

	return dec;
}

/**
 *  struct-declaration:
 *		specifier-qualifer-list struct-declarator-list ;
 *
 *  specifier-qualifier-list:
 *		type-specifier [specifier-qualifier-list]
 *		type-qualifier [specifier-qualifier-list]
 *
 *  struct-declarator-list:
 *		struct-declarator
 *		struct-declarator-list , struct-declarator
 */
StructDeclarationNodePtr DeclarationParser::ParseStructDeclaration(Parser& parser)
{
	auto stDecl = std::make_shared<StructDeclarationNode>(parser.GetTokenizer(), NK_StructDeclaration);

	/// declaration specifiers is a superset of speicifier qualifier list,
	/// for simplicity, just use ParseDeclarationSpecifiers() and check if
	/// there is storage class
	stDecl->specs = ParseDeclarationSpecifiers(parser);
	if (stDecl->specs->stgClasses != NULL)
	{
        assert(0);
		//Error(&stDecl->coord, "Struct/union member should not have storage class");
		stDecl->specs->stgClasses = NULL;
	}
	if (stDecl->specs->tyQuals == NULL && stDecl->specs->tySpecs == NULL)
	{
        assert(0);
		//Error(&stDecl->coord, "Expect type specifier or qualifier");
	}

	// an extension to C89, supports anonymous struct/union member in struct/union
	if (parser.CurrTokenType() == TK_SEMICOLON)
	{
		parser.NextToken();
		return stDecl;
	}

	stDecl->stDecs = ParseStructDeclarator(parser);
	auto prev = stDecl->stDecs;
	while (parser.CurrTokenType() == TK_COMMA)
	{
		parser.NextToken();
		auto node = ParseStructDeclarator(parser);
        prev->next = node;
        prev = node;
	}
	parser.Expect(TK_SEMICOLON);
    parser.NextToken();

	return stDecl;
}

/**
 *  struct-declarator:
 *		declarator
 *		[declarator] : constant-expression
 */
StructDeclaratorNodePtr DeclarationParser::ParseStructDeclarator(Parser& parser)
{
	auto stDec = std::make_shared<StructDeclaratorNode>(parser.GetTokenizer(), NK_StructDeclarator);

	if (parser.CurrTokenType() != TK_COLON)
	{
		stDec->dec = ParseDeclarator(parser, DEC_CONCRETE);
	}
	if (parser.CurrTokenType() == TK_COLON)
	{
        parser.NextToken();
		stDec->expr = ExpressionParser::ParseConstantExpression(parser);
	}

	return stDec;
}

/**
 *  parameter-type-list:
 *		parameter-list
 *		parameter-list , ...
 *
 *  parameter-list:
 *		parameter-declaration
 *		parameter-list , parameter-declaration
 *
 *  parameter-declaration:
 *		declaration-specifiers declarator
 *		declaration-specifiers abstract-declarator
 */
ParameterTypeListNodePtr DeclarationParser::ParseParameterTypeList(Parser& parser)
{
	auto paramTyList = std::make_shared<ParameterTypeListNode>(parser.GetTokenizer(), NK_ParameterTypeList);

	paramTyList->paramDecls = ParseParameterDeclaration(parser);
	auto prev = paramTyList->paramDecls;
	while (parser.CurrTokenType() == TK_COMMA)
	{
        parser.NextToken();
		if (parser.CurrTokenType() == TK_ELLIPSE)
		{
			paramTyList->ellipse = 1;
            parser.NextToken();
			break;
		}
		auto node = ParseParameterDeclaration(parser);
        prev->next = node;
        prev = node;
	}

	return paramTyList;
}

/**
 *  parameter-declaration:
 *		declaration-specifiers declarator
 *		declaration-specifiers abstract-declarator
 */
ParameterDeclarationNodePtr DeclarationParser::ParseParameterDeclaration(Parser& parser)
{
    auto paramDecl = std::make_shared<ParameterDeclarationNode>(parser.GetTokenizer(), NK_ParameterDeclaration);

	paramDecl->specs = ParseDeclarationSpecifiers(parser);
	paramDecl->dec   = ParseDeclarator(parser, DEC_ABSTRACT | DEC_CONCRETE);

	return paramDecl;
}

}
}