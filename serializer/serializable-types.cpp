#include "serializable-types.h"

#include "string.h"
#include <stdio.h>
#include <ctype.h>


b32
serialize_type(String& type_name, void *data, FILE *output)
{
  b32 result = true;

  if (string_eq(type_name, STRING("uint32_t")))
  {
    fprintf(output, "%u", *(u32*)data);
  }
  else
  if (string_eq(type_name, STRING("int32_t")))
  {
    fprintf(output, "%d", *(s32*)data);
  }
  else
  if (string_eq(type_name, STRING("float")))
  {
    // TODO:  Figure out how to maintain precision, perhaps using a hex output.
    fprintf(output, "%f", *(r32*)data);
  }
  else
  if (string_eq(type_name, STRING("char")))
  {
    char c = *(char*)data;
    if (isprint(c) &&
        c != '\\' &&
        c != '\'')
    {
      fprintf(output, "'%c'", *(char*)data);
    }
    else
    if (c == '\n')
    {
      fprintf(output, "'\\n'");
    }
    else
    if (c == '\t')
    {
      fprintf(output, "'\\t'");
    }
    else
    if (c == '\\')
    {
      fprintf(output, "'\\\\'");
    }
    else
    if (c == '\'')
    {
      fprintf(output, "'\\\''");
    }
    else
    {
      fprintf(output, "0x%.2X", *(u32*)data);
    }
  }
  else
  {
    result = false;
  }

  return result;
}