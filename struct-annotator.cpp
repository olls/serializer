#include "struct-annotator.h"

#include "string.h"
#include "serializable-types.h"

#include <stdio.h>


void
print_struct_annotation(StructAnnotation& struct_annotation)
{
  printf("struct %.*s {\n", STR_PRINT(struct_annotation.name));

  for (u32 member_i = 0;
       member_i < struct_annotation.members.n_elements;
       ++member_i)
  {
    StructAnnotationMember& member = struct_annotation.members[member_i];
    printf("  %s %.*s;\n", TYPE_STRINGS[u32(member.type)], STR_PRINT(member.name));
  }

  printf("};\n");
}


void
print_annotated_struct(StructAnnotation& struct_annotation, void *data)
{
  printf("struct %.*s {\n", STR_PRINT(struct_annotation.name));

  for (u32 member_i = 0;
       member_i < struct_annotation.members.n_elements;
       ++member_i)
  {
    StructAnnotationMember& member = struct_annotation.members[member_i];
    printf("  %s %.*s = ", TYPE_STRINGS[u32(member.type)], STR_PRINT(member.name));

    void *member_data = (u8*)data + member.offset;

    switch (member.type)
    {
      case (SerializableType::u32):
        printf("%u", *(u32*)member_data);
        break;
      case (SerializableType::s32):
        printf("%d", *(s32*)member_data);
        break;
      case (SerializableType::r32):
        printf("%f", *(r32*)member_data);
        break;
      case (SerializableType::vec2):
        printf("%f, %f", ((vec2*)member_data)->x, ((vec2*)member_data)->y);
        break;
    }

    printf(";\n");
  }
  printf("};\n");
}