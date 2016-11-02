/**
 Basic preprocessor helpers

 DLM_QUOTE(string)                  - argument-safe string quatation
 DLM_CONCAT(string1, string2)       - argument-safe string concatenation

 DLM_EXTRACT_BITS(x, offset, size)  - get @size bits from @x starting from @offset
 DLM_WRAP_STATEMENT(expr)           - enclose @expr into block

 DLM_MAX, DLM_MIN, DLM_SELECT       - standard max, min, select(ternary operator) functions respectively
*/

#ifndef DLM_ENV_MACRO_H_
#define DLM_ENV_MACRO_H_
#include <limits.h>

/* string manipulations */
#define DLM_QUOTE_(str)         #str
#define DLM_QUOTE(str)          DLM_QUOTE_(str)
#define DLM_CONCAT_(str1, str2) str1 ## str2
#define DLM_CONCAT(str1, str2)  DLM_CONCAT_(str1, str2)

/* ints */
#define DLM_SELECT(cond, a, b)  ( (cond) ? (a) : (b) )
#define DLM_MAX(a, b)           DLM_SELECT( (a)>(b), (a), (b) )
#define DLM_MIN(a, b)           DLM_SELECT( (a)<(b), (a), (b) )
#define DLM_BOOL2INT(cond)      DLM_SELECT( (cond), 1, 0)

/* bit manipulation */
#define DLM_BIT_SIZE(num)                   ( sizeof(num) * CHAR_BIT )
#define DLM_BIT_LOW_MASK(size)              ( (1u << (size)) - 1 )
#define DLM_BIT_HIGH_BITS(x, offset)        ( (x) >> (offset) )
#define DLM_EXTRACT_BITS(x, offset, size)   ( DLM_BIT_HIGH_BITS(x, offset) & DLM_BIT_LOW_MASK(size) )
#define DLM_WRAP_STATEMENT(expr) do { expr } while(0)

#endif /* DLM_ENV_MACRO_H_ */
