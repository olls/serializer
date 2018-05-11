#ifndef ARRAY_STRING_H_DEF
#define ARRAY_STRING_H_DEF

#include "array.h"
#include "string.h"


static inline void
append_string(Array::Array<char>& string, const char *appendee)
{
  Array::add_n(string, (char *)appendee, strlen(appendee));
}


static inline void
append_string(Array::Array<char>& string, String appendee)
{
  Array::add_n(string, (char *)(appendee.start), STR_LENGTH(appendee));
}


#define GET_STRING(string)  (String){.start = (string).elements, .current_position = (string).elements, .end = (string).elements + (string).n_elements}


#endif