#include "cslang/Declaration.h"
#include "cslang/Expression.h"
#include "cslang/Token.h"
#include "cslang/grammer.h"
#include "cslang/Parser.h"
#include "cslang/Token.h"
#include "cslang/predef.h"

namespace
{

std::vector<cslang::ast::TDName> TypedefNames, OverloadNames;

/**
 * Get the identifier declared by the declarator dec
 */
char* GetOutermostID(const cslang::ast::DeclaratorNodePtr& dec)
{
    if (dec->kind == cslang::NK_NameDeclarator)
        return dec->id;

    return GetOutermostID(dec->dec);
}

/**
 * Perform minimum semantic check for each declaration
 */
void PreCheckTypedef(cslang::Parser& parser, const cslang::ast::DeclarationNodePtr& decl)
{
    cslang::NodePtr p = nullptr;
	int sclass = 0;

	if (decl->specs->stgClasses != NULL)
	{
		sclass = std::static_pointer_cast<cslang::ast::TokenNode>(decl->specs->stgClasses)->token;
	}

	p = decl->initDecs;
	while (p != NULL)
	{
        auto dec = std::static_pointer_cast<cslang::ast::InitDeclaratorNode>(p)->dec;
        parser.CheckTypedefName(sclass, GetOutermostID(dec));
		p = p->next;
	}
}

}

namespace cslang
{

int FIRST_StructDeclaration[]   = { FIRST_DECLARATION, 0 };
int FF_StructDeclaration[]      = { FIRST_DECLARATION, TK_RBRACE, 0 };
int FIRST_Function[]            = { FIRST_DECLARATION, TK_LBRACE, 0};
int FIRST_ExternalDeclaration[] = { FIRST_DECLARATION, TK_MUL, TK_LPAREN, 0};

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

/**
 *  translation-unit:
 *		external-declaration
 *		translation-unit external-declaration
 */
TranslationUnitNodePtr DeclarationParser::ParseTranslationUnit(Parser& parser)
{
    auto transUnit = std::make_shared<TranslationUnitNode>(parser.GetTokenizer(), NK_TranslationUnit);
	NodePtr* tail = &transUnit->extDecls;

    // first advance in Parser's ctor
	//parser.NextToken();
	while (parser.CurrTokenType() != TK_END)
	{
		*tail = ParseExternalDeclaration(parser);
		tail = &(*tail)->next;
		//SkipTo(FIRST_ExternalDeclaration, "the beginning of external declaration");
        parser.SkipTo(FIRST_ExternalDeclaration);
	}

	return transUnit;
}

DeclarationNodePtr DeclarationParser::ParseDeclaration(Parser& parser)
{
    auto decl = ParseCommonHeader(parser);
    parser.Expect(TK_SEMICOLON);
    parser.NextToken();
    PreCheckTypedef(parser, decl);

    return decl;
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
#ifdef LANG_GLSL
        while (curr_token == TK_CONST || curr_token == TK_VOLATILE
            || curr_token == TK_IN || curr_token == TK_OUT || curr_token == TK_ATTRIBUTE
            || curr_token == TK_UNIFORM || curr_token == TK_VARYING || curr_token == TK_LAYOUT || curr_token == TK_LOCATION)
#else
		while (curr_token == TK_CONST || curr_token == TK_VOLATILE)
#endif // LANG_GLSL
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
 *      vec2
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
#ifdef LANG_GLSL
    case TK_IN:
    case TK_OUT:
    case TK_ATTRIBUTE:
    case TK_VARYING:
#endif // LANG_GLSL
    {
        auto tok = std::make_shared<TokenNode>(parser.GetTokenizer(), NK_Token);
        tok->token = parser.CurrTokenType();
        *tqTail = tok;
        tqTail = &tok->next;
        parser.NextToken();
    }
		break;

#ifdef LANG_GLSL
    case TK_UNIFORM:
	{
		auto t = parser.PeekToken();
		if (parser.IsTypeName(t))
		{
			auto tok = std::make_shared<TokenNode>(parser.GetTokenizer(), NK_Token);
			tok->token = parser.CurrTokenType();
			*tqTail = tok;
			tqTail = &tok->next;
			parser.NextToken();
		}
		else // ubo
		{
			*tsTail = ParseStructOrUnionOrUniformSpecifier(parser);
			tsTail = &(*tsTail)->next;
			seeTy = 1;
		}
	}
		break;
#endif // LANG_GLSL

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
#ifdef LANG_GLSL
    case TK_BOOL:
    case TK_BOOL2:
    case TK_BOOL3:
    case TK_BOOL4:
    case TK_INT2:
    case TK_INT3:
    case TK_INT4:
    case TK_UINT:
    case TK_UVEC2:
    case TK_UVEC3:
    case TK_UVEC4:
    case TK_FLOAT2:
    case TK_FLOAT3:
    case TK_FLOAT4:
    case TK_MATRIX2:
    case TK_MATRIX3:
    case TK_MATRIX4:
    case TK_SAMPLER_2D:
    case TK_SAMPLER_CUBE:
#endif // LANG_GLSL
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

		*tsTail = ParseStructOrUnionOrUniformSpecifier(parser);
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
StructSpecifierNodePtr DeclarationParser::ParseStructOrUnionOrUniformSpecifier(Parser& parser)
{
	auto stSpec = std::make_shared<StructSpecifierNode>(parser.GetTokenizer(), NK_StructSpecifier);
	switch (parser.CurrTokenType())
	{
	case TK_UNION:
		stSpec->kind = NK_UnionSpecifier;
		break;
	case TK_UNIFORM:
		stSpec->kind = NK_UniformSpecifier;
		break;
	}

    NodePtr prev = nullptr;

	parser.NextToken();
	switch (parser.CurrTokenType())
	{
	case TK_ID:
		stSpec->id = (char*)(parser.GetTokenizer().GetTokenVal().p);
		parser.NextToken();

		parser.CheckTypedefName(TK_TYPEDEF, stSpec->id);

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
		//Error(&TokenCoord, "parser.Expect identifier or { after struct/union");
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
		//Error(&TokenCoord, "parser.Expect identifier or { after enum");
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

			if (parser.IsTypeName(parser.CurrToken()))
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
#ifdef LANG_GLSL
				else if (parser.CurrTokenType() == TK_LAYOUT)
				{
					int zz = 0;
				}
#endif // LANG_GLSL
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
 *  initializer:
 *		assignment-expression
 *		{ initializer-list }
 *		{ initializer-list , }
 *
 *  initializer-list:
 *		initializer
 *		initializer-list, initializer
 */
InitializerNodePtr DeclarationParser::ParseInitializer(Parser& parser)
{
    NodePtr* tail = nullptr;

    auto init = std::make_shared<InitializerNode>(parser.GetTokenizer(), NK_Initializer);
	if (parser.CurrTokenType() == TK_LBRACE)
	{
		init->lbrace = 1;
        parser.NextToken();
		init->initials = ParseInitializer(parser);
		tail = &init->initials->next;
		while (parser.CurrTokenType() == TK_COMMA)
		{
            parser.NextToken();
			if (parser.CurrTokenType() == TK_RBRACE)
				break;
			*tail = ParseInitializer(parser);
			tail = &(*tail)->next;
		}
		parser.Expect(TK_RBRACE);
        parser.NextToken();
	}
	else
	{
		init->lbrace = 0;
		init->expr = ExpressionParser::ParseAssignmentExpression(parser);
	}

	return init;
}

/**
 *  init-declarator:
 *		declarator
 *      declarator = initializer
 */
InitDeclaratorNodePtr DeclarationParser::ParseInitDeclarator(Parser& parser)
{
	auto initDec = std::make_shared<InitDeclaratorNode>(parser.GetTokenizer(), NK_InitDeclarator);
	initDec->dec = ParseDeclarator(parser, DEC_CONCRETE);
	if (parser.CurrTokenType() == TK_ASSIGN)
	{
        parser.NextToken();
		initDec->init = ParseInitializer(parser);
	}

	return initDec;
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
        // fixme
		//if (kind == DEC_ABSTRACT)
		//{
  //          assert(0);
		//	//Error(&TokenCoord, "Identifier is not permitted in the abstract declarator");
		//}

		dec->id = (char*)(parser.GetTokenizer().GetTokenVal().p);
		parser.NextToken();
	}
#ifdef LANG_GLSL
	else if (parser.CurrTokenType() == TK_LAYOUT)
	{
		parser.NextToken();
		parser.Expect(TK_LPAREN);

		parser.NextToken();
		parser.Expect(TK_LOCATION);

		parser.NextToken();
		parser.Expect(TK_ASSIGN);

		parser.NextToken();
		parser.Expect(TK_INTCONST);

		parser.NextToken();
		parser.Expect(TK_RPAREN);

		parser.NextToken();
		parser.Expect(TK_IN);

		parser.NextToken();

		parser.NextToken();
		parser.Expect(TK_ID);
		dec->id = (char*)(parser.GetTokenizer().GetTokenVal().p);

		parser.NextToken();
	}
#endif // LANG_GLSL
	else if (kind == DEC_CONCRETE)
	{
        assert(0);
		//Error(&TokenCoord, "parser.Expect identifier");
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
		//Error(&stDecl->coord, "parser.Expect type specifier or qualifier");
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


/**
 *  The function defintion and declaration have some common parts:
 *	declaration-specifiers [init-declarator-list]
 *  if we found that the parts followed by a semicolon, then it is a declaration
 *  or if the init-declarator list is a stand-alone declarator, then it may be
 *  a function definition.
 */
DeclarationNodePtr DeclarationParser::ParseCommonHeader(Parser& parser)
{
    NodePtr* tail = nullptr;

    auto decl = std::make_shared<DeclarationNode>(parser.GetTokenizer(), NK_Declaration);
	decl->specs = ParseDeclarationSpecifiers(parser);
	if (parser.CurrTokenType() != TK_SEMICOLON)
	{
		decl->initDecs = ParseInitDeclarator(parser);
		tail = &decl->initDecs->next;
		while (parser.CurrTokenType() == TK_COMMA)
		{
			parser.NextToken();
			*tail = ParseInitDeclarator(parser);
			tail = &(*tail)->next;
		}
	}

	return decl;
}

/**
 * If initDec is a legal function declarator, return it
 */
FunctionDeclaratorNodePtr DeclarationParser::GetFunctionDeclarator(const InitDeclaratorNodePtr& initDec)
{
	DeclaratorNodePtr dec = nullptr;

	if (initDec == NULL || initDec->next != NULL || initDec->init != NULL)
		return NULL;

	dec = initDec->dec;
	while (dec && dec->kind != NK_FunctionDeclarator)
		dec = dec->dec;

	if (dec == NULL || dec->dec->kind != NK_NameDeclarator)
		return NULL;

	return std::static_pointer_cast<FunctionDeclaratorNode>(dec);
}

/**
 *  external-declaration:
 *		function-definition
 *		declaration
 *
 *  function-definition:
 *		declaration-specifiers declarator [declaration-list] compound-statement
 *
 *  declaration:
 *		declaration-specifiers [init-declarator-list] ;
 *
 *  declaration-list:
 *		declaration
 *		declaration-list declaration
 */
NodePtr DeclarationParser::ParseExternalDeclaration(Parser& parser)
{
	FunctionDeclaratorNodePtr fdec = nullptr;

    DeclarationNodePtr decl = ParseCommonHeader(parser);
    InitDeclaratorNodePtr initDec = std::static_pointer_cast<InitDeclaratorNode>(decl->initDecs);
	if (decl->specs->stgClasses != NULL && std::static_pointer_cast<TokenNode>(decl->specs->stgClasses)->token == TK_TYPEDEF)
		goto not_func;

	fdec = GetFunctionDeclarator(initDec);
	if (fdec != NULL)
	{
		if (parser.CurrTokenType() == TK_SEMICOLON)
		{
			parser.NextToken();
			if (parser.CurrTokenType() != TK_LBRACE)
				return decl;

			// maybe a common error, function definition follows ;
			//Error(&decl->coord, "maybe you accidently add the ;");
            assert(0);
		}
		else if (fdec->paramTyList && parser.CurrTokenType() != TK_LBRACE)
		{
			// a common error, function declaration loses ;
			goto not_func;
		}

        auto func = std::make_shared<FunctionNode>(parser.GetTokenizer(), NK_Function);

		func->coord = decl->coord;
		func->specs = decl->specs;
		func->dec = initDec->dec;
		func->fdec = fdec;

        parser.IncreaseLevle();
		if (func->fdec->paramTyList)
		{
			NodePtr p = func->fdec->paramTyList->paramDecls;
			while (p)
			{
				parser.CheckTypedefName(0, GetOutermostID((std::static_pointer_cast<ParameterDeclarationNode>(p))->dec));
				p = p->next;
			}
		}
#ifdef FEATURE_DECL_BEFORE_STAT
        NodePtr* tail = &func->decls;
		while (CurrentTokenIn(parser.CurrTokenType(), FIRST_Declaration))
		{
			*tail = ParseDeclaration(parser);
			tail = &(*tail)->next;
		}
#endif // FEATURE_DECL_BEFORE_STAT
        parser.DecreaseLevle();

		func->stmt = StatementParser::ParseCompoundStatement(parser);

		return func;
	}

not_func:
	parser.Expect(TK_SEMICOLON);
    parser.NextToken();
	PreCheckTypedef(parser, decl);

	return decl;
}

}
}