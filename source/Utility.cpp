#include "cslang/Utility.h"
#include "cslang/Token.h"
#include "cslang/Type.h"

#include <stdarg.h>
#include <assert.h>

namespace cslang
{

std::string StringFormat(const std::string fmt, ...)
{
	int size = ((int)fmt.size()) * 2 + 50;   // Use a rubric appropriate for your code
	std::string str;
	va_list ap;
	while (1) {     // Maximum two passes on a POSIX system...
		str.resize(size);
		va_start(ap, fmt);
		int n = vsnprintf((char *)str.data(), size, fmt.c_str(), ap);
		va_end(ap);
		if (n > -1 && n < size) {  // Everything worked
			str.resize(n);
			return str;
		}
		if (n > -1)  // Needed size returned
			size = n + 1;   // For null char
		else
			size *= 2;      // Guess at a larger size (OS specific)
	}
	return str;
}

int TokenTypeToSymbolType(unsigned int token)
{
    int type = 0;
    switch (token)
    {
    case TK_CHAR:
        type = CHAR;
        break;
    case TK_SHORT:
        type = SHORT;
        break;
    case TK_INT:
        type = INT;
        break;
    case TK_INT64:
        // fixme
        type = INT;
        break;
    case TK_LONG:
        type = LONG;
        break;
    case TK_FLOAT:
        type = FLOAT;
        break;
    case TK_DOUBLE:
        type = DOUBLE;
        break;
    case TK_SIGNED:
        type = INT;
        break;
    case TK_UNSIGNED:
        type = UINT;
        break;
#ifdef LANG_GLSL
    case TK_BOOL:
        type = BOOL;
        break;
    case TK_BOOL2:
        type = BOOL2;
        break;
    case TK_BOOL3:
        type = BOOL3;
        break;
    case TK_BOOL4:
        type = BOOL4;
        break;
    case TK_INT2:
        type = INT2;
        break;
    case TK_INT3:
        type = INT3;
        break;
    case TK_INT4:
        type = INT4;
        break;
    case TK_UINT:
        type = UINT;
        break;
    case TK_UVEC2:
        type = UVEC2;
        break;
    case TK_UVEC3:
        type = UVEC3;
        break;
    case TK_UVEC4:
        type = UVEC4;
        break;
    case TK_FLOAT2:
        type = FLOAT2;
        break;
    case TK_FLOAT3:
        type = FLOAT3;
        break;
    case TK_FLOAT4:
        type = FLOAT4;
        break;
    case TK_MATRIX2:
        type = MATRIX2;
        break;
    case TK_MATRIX3:
        type = MATRIX3;
        break;
    case TK_MATRIX4:
        type = MATRIX4;
        break;
    default:
        assert(0);
#endif // LANG_GLSL
    }
    return type;
}

std::string PrintFloatConstant(float f)
{
    char format[1024];
    snprintf(format, sizeof format, "%g", f);
    if (!strchr(format, '.') && !strchr(format, 'e')) {
        return StringFormat("%g.0", f);
    } else {
        return StringFormat("%s", format);
    }
}

std::string PrintDoubleConstant(double d)
{
    char format[1024];
    snprintf(format, sizeof format, "%g", d);
    if (!strchr(format, '.') && !strchr(format, 'e')) {
        return StringFormat("%g.0", d);
    } else {
        return StringFormat("%s", format);
    }
}

}