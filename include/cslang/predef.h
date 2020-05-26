#pragma once

//// c89: Declaration have to be before statements in a block.
//#define FEATURE_DECL_BEFORE_STAT

//#define LANG_C89
//#define LANG_C99
#define LANG_GLSL

#ifdef LANG_C89
#define FEATURE_DECL_BEFORE_STAT
#endif // LANG_C89