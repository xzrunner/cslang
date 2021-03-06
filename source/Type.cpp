#include "cslang/type.h"
#include "cslang/Utility.h"

#include <stdarg.h>
#include <stdlib.h>
#include <vadefs.h>
#include <assert.h>

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

namespace cslang
{

Type WCharType = Types[WCHAR];
Type Types[VOID - CHAR + 1];

/**
 * Although int and long are different types from C language, but from some underlying implementations,
 * the size and representation of them maybe identical. Actually, the function should be provided by
 * the target. UCC defines a series of type code:
 * I1: signed 1 byte       U1: unsigned 1 byte
 * I2: signed 2 byte       U2: unsigned 2 byte
 * I4: signed 4 byte       U4: unsigned 4 byte
 * F4: 4 byte floating     F8: 8 byte floating
 * V: no type              B: memory block, used for struct/union and array.
 * The target should provide TypeCode to define the map from type category to type code. And UCC can
 * add more type codes on demand of different targets.
 */
int TypeCode(const Type& ty)
{
	static int optypes[] = {
#define TYPE(type, size, name) size,
#include "cslang/type_cfg.h"
#undef TYPE
    };

	assert(ty.categ != FUNCTION);
	return optypes[ty.categ];
}

/**
 * Get a type's unqualified version
 */
const Type* Unqual(const Type& ty)
{
    if (ty.qual) {
//        ty = ty.bty;
    }
    return &ty;
}

/**
 * Construct an array type, len is the array length.
 */
std::unique_ptr<Type> ArrayOf(int len, const Type& ty)
{
    auto aty = std::make_unique<Type>();

    aty->categ = ARRAY;
    aty->qual = 0;
    aty->size = len * ty.size;
    aty->align = ty.align;
    aty->b_categ = ty.categ;

    return aty;
}

/**
 * Get the type's user-readable string.
 */
std::string TypeToString(const Type& ty)
{
	int qual = 0;
	char *str = nullptr;
	char *names[] =
	{
#define TYPE(type, size, name) name,
#include "cslang/type_cfg.h"
#undef TYPE
	};

	if (ty.qual != 0)
	{
		qual = ty.qual;
		auto _ty = Unqual(ty);

		if (qual == CONST)
			str = "const";
		else if (qual == VOLATILE)
			str = "volatile";
		else
			str = "const volatile";

		return StringFormat("%s %s", str, TypeToString(*_ty));
	}

	if (ty.categ >= CHAR && ty.categ <= LONGDOUBLE && ty.categ != ENUM)
		return names[ty.categ];

	switch (ty.categ)
	{
	case ENUM:
		return StringFormat("enum %s", static_cast<const EnumType&>(ty).id.c_str());

	case POINTER:
		//return StringFormat("%s *", TypeToString(ty.bty));
        assert(0);
        return nullptr;

	case UNION:
		return StringFormat("union %s", static_cast<const RecordType&>(ty).id.c_str());

	case STRUCT:
		return StringFormat("struct %s", static_cast<const RecordType&>(ty).id.c_str());

	case ARRAY:
		//return StringFormat("%s[%d]", TypeToString(ty.bty), ty.size / ty.bty->size);
        assert(0);
        return nullptr;

	case VOID:
		return "void";

	case FUNCTION:
		//return StringFormat("%s ()", TypeToString(fty->bty));
        assert(0);
        return nullptr;

	default:
		assert(0);
		return NULL;
	}

    return nullptr;
}

void SetupTypeSystem()
{
    Types[CHAR].size = Types[UCHAR].size = CHAR_SIZE;
    Types[SHORT].size = Types[USHORT].size = SHORT_SIZE;
    Types[INT].size = Types[UINT].size = INT_SIZE;
    Types[LONG].size = Types[ULONG].size = LONG_SIZE;
    Types[LONGLONG].size = Types[ULONGLONG].size = LONG_LONG_SIZE;
    Types[FLOAT].size = FLOAT_SIZE;
    Types[DOUBLE].size = DOUBLE_SIZE;
    Types[LONGDOUBLE].size = LONG_DOUBLE_SIZE;
    Types[POINTER].size = INT_SIZE;

    for (int i = CHAR; i <= VOID; ++i)
    {
        Types[i].categ = i;
        Types[i].align = Types[i].size;
    }

    //FunctionType fty;
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