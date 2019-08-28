#include "vexc/type.h"

#include <stdarg.h>
#include <stdlib.h>
#include <vadefs.h>

#define CHAR_SIZE         1
#define SHORT_SIZE        2
#define INT_SIZE          4
#define LONG_SIZE         4
#define LONG_LONG_SIZE    4
#define FLOAT_SIZE        4
#define DOUBLE_SIZE       8
#define LONG_DOUBLE_SIZE  8

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

Type WCharType = Types[WCHAR];
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
    aty.b_categ = ty.categ;

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

void SetupTypeSystem()
{
    int i;
    FunctionType fty;

    Types[CHAR].size = Types[UCHAR].size = CHAR_SIZE;
    Types[SHORT].size = Types[USHORT].size = SHORT_SIZE;
    Types[INT].size = Types[UINT].size = INT_SIZE;
    Types[LONG].size = Types[ULONG].size = LONG_SIZE;
    Types[LONGLONG].size = Types[ULONGLONG].size = LONG_LONG_SIZE;
    Types[FLOAT].size = FLOAT_SIZE;
    Types[DOUBLE].size = DOUBLE_SIZE;
    Types[LONGDOUBLE].size = LONG_DOUBLE_SIZE;
    Types[POINTER].size = INT_SIZE;

    for (i = CHAR; i <= VOID; ++i)
    {
        Types[i].categ = i;
        Types[i].align = Types[i].size;
    }

    //ALLOC(fty);
    //fty->categ = FUNCTION;
    //fty->qual = 0;
    //fty->align = fty.size = Types[POINTER].size;
    //fty->bty = Types[INT);
    //ALLOC(fty->sig);
    //CALLOC(fty->sig->params);
    //fty->sig->hasProto = 0;
    //fty->sig->hasEllipse = 0;

    //DefaultFunctionType = (Type)fty;
    //WCharType = Types[WCHAR);
}

}