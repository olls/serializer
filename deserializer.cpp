#include "deserializer.h"

#include "ini-parser.h"
#include "value-parser.h"
#include "serializable-types.h"
#include "parse.h"


b32
parse_value(String text, SerializableType type, void *result)
{
  b32 success = true;

  switch (type)
  {
    case (SerializableType::u32):
    {
      success &= parse_u32(&text, (u32*)result);
    } break;
    case (SerializableType::s32):
    {
      success &= parse_s32(&text, (s32*)result);
    } break;
    case (SerializableType::r32):
    {
      success &= parse_r32(&text, (r32*)result);
    } break;
    case (SerializableType::vec2):
    {
      // TODO:  Temporary manually parse vec2, use recursive struct deserialisation once implemented
      success &= parse_r32(&text, &((vec2*)result)->x);
      CONSUME_WHILE(text, is_space_tabs);
      success &= parse_r32(&text, &((vec2*)result)->y);
    } break;
  }

  return success;
}


b32
deserialize_struct(String text, StructAnnotation& struct_annotation, void *result)
{
  b32 success = true;

  String struct_section = find_section(text, struct_annotation.name);
  if (struct_section.start == 0)
  {
    printf("Could not find stuct section on deserialize\n");
    success = false;
  }
  else
  {
    // printf("Found section: \"%.*s\"\n\n", STR_PRINT(struct_section));

    for (u32 member_i = 0;
         member_i < struct_annotation.members.n_elements;
         ++member_i)
    {
      StructAnnotationMember& member = struct_annotation.members[member_i];

      String value_string = get_value(struct_section, member.name);
      if (value_string.start == 0)
      {
        printf("Could not find value on deserialize\n");
      }
      else
      {
        // printf("%s %.*s: \"%.*s\"\n", TYPE_STRINGS[u32(member.type)], STR_PRINT(member.name), STR_PRINT(value_string));

        void *member_data = (u8*)result + member.offset;
        b32 success = parse_value(value_string, member.type, member_data);
        if (!success)
        {
          printf("Failed to parse %.*s value\n", STR_PRINT(member.name));
        }
      }
    }
  }

  return success;
}