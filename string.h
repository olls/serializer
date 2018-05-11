#ifndef STRING_H_DEF
#define STRING_H_DEF

#include "types.h"

#include <string.h>  // strlen


struct String
{
  const char *start;
  const char *current_position;
  const char *end;
};


struct WriteString
{
  char *start;
  char *current_position;
  char *end;
};


#define STR_SET(string, c_string)  ((string).start = (c_string)); \
                                   ((string).end = (string).start + strlen(c_string)); \
                                   ((string).current_position = (string).start)

#define STR_LENGTH(string)  s32((string).end - (string).start)
#define STR_PRINT(string)  STR_LENGTH(string), ((string).start)

#define SPACE_LEFT(string)  s32((string).end - (string).current_position)


static b32
append_string(WriteString &string, const char *appendee)
{
  b32 success = true;

  s32 length = strlen(appendee);
  if (length >= SPACE_LEFT(string))
  {
    success = false;
  }
  else
  {
    strcpy(string.current_position, appendee);
    string.current_position += length;
  }

  return success;
}


static b32
append_string(WriteString &string, String appendee)
{
  b32 success = true;

  if (STR_LENGTH(appendee) >= SPACE_LEFT(string))
  {
    success = false;
  }
  else
  {
    strncpy(string.current_position, appendee.start, STR_LENGTH(appendee));
    string.current_position += STR_LENGTH(appendee);
  }

  return success;
}


#endif