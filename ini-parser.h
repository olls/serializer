#ifndef INI_PARSER_H_DEF
#define INI_PARSER_H_DEF

#include "types.h"
#include "string.h"


String
find_line_starting_with(String text, String prefix);


String
find_section(String text, String name);


String
get_value(String text, String name);


#endif