#include "vexc/type.h"

#include <stdarg.h>
#include <stdlib.h>
#include <vadefs.h>

#ifdef _WIN32
#define	WCHAR		USHORT
#else
#define WCHAR		ULONG
#endif

namespace
{

///**
// * Format a name
// */
//char* FormatName(const char *fmt, ...)
//{
//	char buf[256];
//	va_list ap;
//
//	va_start(ap, fmt);
//	vsprintf(buf, fmt, ap);
//	va_end(ap);
//
//	return InternName(buf, strlen(buf));
//}

}

namespace vexc
{

Type WCharType = T(WCHAR);
Type Types[VOID - CHAR + 1];

/**
 * Get a type's unqualified version
 */
Type Unqual(const Type& ty)
{
    if (ty.qual) {
//        ty = ty.bty;
    }
    return ty;
}

/**
 * Construct an array type, len is the array length.
 */
Type ArrayOf(int len, Type ty)
{
    Type aty;

    aty.categ = ARRAY;
    aty.qual = 0;
    aty.size = len * ty.size;
    aty.align = ty.align;
//    aty.bty = ty;

    return (Type)aty;
}

/**
 * Get the type's user-readable string.
 */
char* TypeToString(const Type& ty)
{
	int qual;
	char *str;
	char *names[] =
	{
		"char", "unsigned char", "short", "unsigned short", "int", "unsigned int",
		"long", "unsigned long", "long long", "unsigned long long", "enum", "float", "double",
		"long double"
	};

	//if (ty.qual != 0)
	//{
	//	qual = ty.qual;
	//	ty = Unqual(ty);

	//	if (qual == CONST)
	//		str = "const";
	//	else if (qual == VOLATILE)
	//		str = "volatile";
	//	else
	//		str = "const volatile";

	//	return FormatName("%s %s", str, TypeToString(ty));
	//}

	if (ty.categ >= CHAR && ty.categ <= LONGDOUBLE && ty.categ != ENUM)
		return names[ty.categ];

	//switch (ty.categ)
	//{
	//case ENUM:
	//	return FormatName("enum %s", ((EnumType)ty)->id);

	//case POINTER:
	//	return FormatName("%s *", TypeToString(ty.bty));

	//case UNION:
	//	return FormatName("union %s", ((RecordType)ty)->id);

	//case STRUCT:
	//	return FormatName("struct %s", ((RecordType)ty)->id);

	//case ARRAY:
	//	return FormatName("%s[%d]", TypeToString(ty.bty), ty.size / ty.bty->size);

	//case VOID:
	//	return "void";

	//case FUNCTION:
	//	{
	//		FunctionType fty = (FunctionType)ty;

	//		return FormatName("%s ()", TypeToString(fty->bty));
	//	}

	//default:
	//	assert(0);
	//	return NULL;
	//}

    return nullptr;
}

}