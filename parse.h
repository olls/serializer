#ifndef PARSE_H_DEF
#define PARSE_H_DEF

#include "string.h"


String
get_next_line(String *text);


b32
string_starts_with(String text, String value);


static inline b32
is_space_tabs(char character)
{
  return (character == ' ' ||
          character == '\t');
}


#define CONSUME_WHILE(string, func) while ((func)((string).current_position[0]) && (string).current_position < (string).end) {(string).current_position += 1;}
#define CONSUME_UNTIL(string, func) while (!(func)((string).current_position[0]) && (string).current_position < (string).end) {(string).current_position += 1;}
#define CONSUME_UNTIL_CHAR(string, character) while (((string).current_position[0] != (character)) && (string).current_position < (string).end) {(string).current_position += 1;}


#endif