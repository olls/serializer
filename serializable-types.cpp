#include "serializable-types.h"

#include "string.h"
#include <stdio.h>


b32
serialize_type(String& type_name, void *data, FILE *output)
{
  b32 result = true;

  if (string_eq(type_name, STRING("u32")))
  {
    fprintf(output, "%u", *(u32*)data);
  }
  else
  if (string_eq(type_name, STRING("s32")))
  {
    fprintf(output, "%d", *(s32*)data);
  }
  else
  if (string_eq(type_name, STRING("r32")))
  {
    fprintf(output, "%f", *(r32*)data);
  }
  else
  if (string_eq(type_name, STRING("char")))
  {
    fprintf(output, "'%c'", *(char*)data);
  }
  else
  {
    result = false;
  }

  return result;
}