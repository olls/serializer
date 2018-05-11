#include "ini-parser.h"

#include "parse.h"
#include "array.h"
#include "array-string.h"

#include <stdio.h>


String
find_line_starting_with(String text, String prefix)
{
  String result = {};

  b32 found_prefix = false;
  b32 eof = false;
  String line = {};
  text.current_position = text.start;

  while (!found_prefix &&
         !eof)
  {
    line = get_next_line(&text);

    if (string_starts_with(line, prefix))
    {
      found_prefix = true;
    }

    if (text.current_position >= text.end)
    {
      break;
    }
  }

  if (found_prefix)
  {
    result = line;
  }

  return result;
}


String
find_section(String text, String name)
{
  String result = {};

  // Find section heading

  Array::Array<char> heading = {};
  heading += '[';
  Array::add_n(heading, (char*)name.start, STR_LENGTH(name));
  heading += ']';

  String heading_line = find_line_starting_with(text, GET_STRING(heading));
  Array::free_array(heading);

  if (heading_line.start != 0)
  {
    // Find end of section

    result.start = heading_line.end + 1;
    result.current_position = result.start;
    result.end = text.end;

    b32 found_end = false;
    String line = {};

    while (!found_end)
    {
      line = get_next_line(&result);
      if (result.current_position >= result.end ||
          string_starts_with(line, STRING("[")))
      {
        found_end = true;
      }
    }
    result.end = result.current_position;
  }

  return result;
}


String
get_value(String text, String name)
{
  String result = {};

  String line = find_line_starting_with(text, name);

  if (line.start != 0)
  {
    line.current_position = line.start;

    CONSUME_WHILE(line, is_space_tabs);

    CONSUME_UNTIL_CHAR(line, ':');
    line.current_position += 1;

    CONSUME_WHILE(line, is_space_tabs);

    if (line.current_position < line.end)
    {
      result.start = line.current_position;
      result.current_position = line.current_position;
      result.end = line.end;
    }
  }

  return result;
}