#include "serializable-types.h"

#include <stdio.h>
#include <ctype.h>
#include <string.h>


b32
serialize_type(const char *type_name, void *data, FILE *output)
{
  b32 result = true;

  if (strcmp(type_name, "uint32_t") == 0)
  {
    fprintf(output, "%u", *(u32*)data);
  }
  else
  if (strcmp(type_name, "int32_t") == 0)
  {
    fprintf(output, "%d", *(s32*)data);
  }
  else
  if (strcmp(type_name, "float") == 0)
  {
    // TODO:  Figure out how to maintain precision, perhaps using a hex output.
    fprintf(output, "%f", *(r32*)data);
  }
  else
  if (strcmp(type_name, "char") == 0)
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