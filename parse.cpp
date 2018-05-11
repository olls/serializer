#include "parse.h"


// Returns a String containing the text between the current current_position and the next line-break or end of the input String.
//
// text.current_position is moved to the end of the returned line.
//
//
String
get_next_line(String *text)
{
  String result = {};
  result.start = text->current_position;

  while (text->current_position != text->end &&
         text->current_position[0] != '\n')
  {
    text->current_position += 1;
  }

  result.end = text->current_position;

  // Move current position past the new line character
  if (text->current_position[0] == '\n')
  {
    text->current_position += 1;
  }

  return result;
}


b32
string_starts_with(String text, const char value[])
{
  b32 result = true;

  if (STR_LENGTH(text) == 0)
  {
    result = false;
  }

  for (u32 i = 0;
       i < STR_LENGTH(text);
       ++i)
  {
    if (value[i] == '\0')
    {
      break;
    }

    if (text.start[i] != value[i])
    {
      result = false;
      break;
    }
  }

  return result;
}