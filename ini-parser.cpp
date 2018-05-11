#include "ini-parser.h"

#include "parse.h"
#include "array.h"

#include <stdio.h>


String
find_line_starting_with(String text, const char prefix[])
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
  heading += '\0';

  String heading_line = find_line_starting_with(text, heading.elements);
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
          string_starts_with(line, "["))
      {
        found_end = true;
      }
    }
    result.end = result.current_position;
  }

  return result;
}


String
get_value(String text, const char name[])
{
  String result = {};

  String line = find_line_starting_with(text, name);

  if (line.start != 0)
  {
    line.current_position = line.start;

    consume_space_tabs(line);

    while (line.current_position[0] != ':')
      line.current_position += 1;
    line.current_position += 1;

    consume_space_tabs(line);

    if (line.current_position < line.end)
    {
      result.start = line.current_position;
      result.current_position = line.current_position;
      result.end = line.end;
    }
  }

  return result;
}