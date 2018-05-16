#include "struct-annotator.h"

#include "string.h"
#include "serializable-types.h"
#include "maths.h"

#include <stdio.h>


StructAnnotation *
get_struct_annotation(StructAnnotations& struct_annotations, String annotation_name)
{
  return Hashmap::get(struct_annotations.map, annotation_name.start, STR_LENGTH(annotation_name));
}


void
print_struct_annotation(String& struct_annotation_name, StructAnnotations& struct_annotations, u32 indent)
{
  StructAnnotation *struct_annotation = get_struct_annotation(struct_annotations, struct_annotation_name);

  if (struct_annotation == NULL)
  {
    printf("%.*s", STR_PRINT(struct_annotation_name));
  }
  else
  {
    printf("struct %.*s {\n", STR_PRINT(struct_annotation->name));

    for (u32 member_i = 0;
         member_i < struct_annotation->members.n_elements;
         ++member_i)
    {
      StructAnnotationMember& member = struct_annotation->members[member_i];
      printf("%*s", 2*(indent + 1), "");

      print_struct_annotation(member.type_name, struct_annotations, indent + 1);

      printf(" %.*s;\n", STR_PRINT(member.name));
    }

    printf("%*s}", 2*indent, "");
  }

  if (indent == 0)
  {
    printf(";\n");
  }
}


void
print_annotated_struct(String& struct_annotation_name, void *data, StructAnnotations& struct_annotations, u32 indent)
{
  StructAnnotation *struct_annotation = get_struct_annotation(struct_annotations, struct_annotation_name);

  if (struct_annotation == NULL)
  {
    b32 serialized = serialize_type(struct_annotation_name, data);

    if (!serialized)
    {
      printf("[No %.*s annotation]", STR_PRINT(struct_annotation_name));
    }
  }
  else
  {

    printf("struct %.*s {\n", STR_PRINT(struct_annotation->name));

    for (u32 member_i = 0;
         member_i < struct_annotation->members.n_elements;
         ++member_i)
    {
      StructAnnotationMember& member = struct_annotation->members[member_i];
      printf("%*s%.*s %.*s = ", 2*(indent + 1), "", STR_PRINT(member.type_name), STR_PRINT(member.name));

      void *member_data = (u8*)data + member.offset;

      print_annotated_struct(member.type_name, member_data, struct_annotations, indent + 1);
      printf(";\n");
    }
    printf("%*s}", 2*indent, "");

  }

  if (indent == 0)
  {
    printf(";\n");
  }
}