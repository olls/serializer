#ifndef VALUE_PARSER_H_DEF
#define VALUE_PARSER_H_DEF


#include "types.h"
#include "string.h"


b32
is_num(char character);


b32
parse_u32(String *text, u32 *result);


b32
parse_s32(String *text, s32 *result);


b32
parse_r32(String *text, r32 *result);


b32
parse_char(String *text, char *result);


#endif