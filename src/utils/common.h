#ifndef H_UTIL_INCLUDED
#define H_UTIL_INCLUDED
#define stage_fn_t void(*)(int*, GameSettings*)
#define stage_fn_var(name) void(*name)(int*, GameSettings*)
#define stage_invoke(stage, settings) if (stages[stage]) (*stages[stage])(&stage, settings)

#define arrlen(type, arr) sizeof(arr) / sizeof(type)

#define inRangeX(n, min, max) min < n && n < max
#define inRange(n, min, max) min <= n && n <= max
#define inRangeE(n, min, max) min <= n && n < max
#define inRangeI(n, min, max) min < n && n <= max

#define center(l, n) (l - n) / 2

#include <string.h>

#define pad(a, b) ((b - a)  / 2)
#define padlen(text, max) pad(strlen(text), max)
#define formatCenter(text, padlen)  "%*s%s%*s", padlen, "", text, padlen, ""

#include <math.h>

#define numlen(n) (floor(log10(abs(n))) + 1)
typedef unsigned int uint;
typedef long long unsigned llu;
#endif