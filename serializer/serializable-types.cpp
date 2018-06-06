#include "serializable-types.h"

#include <stdio.h>
#include <ctype.h>
#include <string.h>


b32
serializable_type(const char *type_name, SerializableType *result)
{
  b32 success = true;

  if (strcmp(type_name, "uint32_t") == 0)
  {
    *result = SerializableType__uint32_t;
  }
  else
  if (strcmp(type_name, "int32_t") == 0)
  {
    *result = SerializableType__int32_t;
  }
  else
  if (strcmp(type_name, "float") == 0)
  {
    *result = SerializableType__float;
  }
  else
  if (strcmp(type_name, "char") == 0)
  {
    *result = SerializableType__char;
  }
  else
  {
    success = false;
  }

  return success;
}


void
serialize_type(SerializableType type, void *data, FILE *output)
{
  b32 result = true;

  switch (type)
  {
    case (SerializableType__uint32_t):
    {
      fprintf(output, "%u", *(u32*)data);
    } break;

    case (SerializableType__int32_t):
    {
      fprintf(output, "%d", *(s32*)data);
    } break;

    case (SerializableType__float):
    {
      // TODO:  Figure out how to maintain precision, perhaps using a hex output.
      fprintf(output, "%f", *(r32*)data);
    } break;

    case (SerializableType__char):
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
        fprintf(output, "0x%.2X", *(char*)data);
      }
    } break;
  }
}


u32
serializable_type_size(SerializableType type)
{
  u32 result;

  switch (type)
  {
    case (SerializableType__uint32_t):
    {
      result = sizeof(uint32_t);
    } break;
    case (SerializableType__int32_t):
    {
      result = sizeof(int32_t);
    } break;
    case (SerializableType__float):
    {
      result = sizeof(float);
    } break;
    case (SerializableType__char):
    {
      result = sizeof(char);
    } break;
  }

  return result;
}
