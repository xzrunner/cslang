#include "vexc/Declaration.h"
#include "vexc/Token.h"
#include "vexc/grammer.h"

namespace vexc
{
namespace ast
{

int FIRST_Declaration[] = { FIRST_DECLARATION, 0};

bool DeclarationParser::IsTypeName(int tok)
{
    return /*tok == TK_ID ? IsTypedefName(TokenValue.p) :*/ (tok >= TK_AUTO && tok <= TK_VOID);
}

bool DeclarationParser::IsTypedefName(const char* id)
{
	//Vector v = TypedefNames;
	//TDName tn;

	//FOR_EACH_ITEM(TDName, tn, v)
	//	if (tn->id == id && tn->level <= Level && ! tn->overload)
	//		return 1;
	//ENDFOR

	return false;
}

}
}