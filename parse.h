#ifndef PARSE_H_DEF
#define PARSE_H_DEF

#include "string.h"


String
get_next_line(String *text);


b32
string_starts_with(String text, const char value[]);


static inline void
consume_space_tabs(String& text)
{
  while (text.current_position[0] == ' ' ||
       text.current_position[0] == '\t')
  {
    text.current_position += 1;
  }
}


#define CONSUME_WHILE(string, func) while ((func)((string).current_position[0]) && (string).current_position < (string).end) {(string).current_position += 1;}
#define CONSUME_UNTIL(string, func) while (!(func)((string).current_position[0]) && (string).current_position < (string).end) {(string).current_position += 1;}


#endif