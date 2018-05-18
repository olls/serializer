#include "struct-annotator.h"

#include "string.h"
#include "serializable-types.h"

#include <stdio.h>


StructAnnotation *
get_struct_annotation(StructAnnotations& struct_annotations, String annotation_name)
{
  return Hashmap::get(struct_annotations.map, annotation_name.start, STR_LENGTH(annotation_name));
}


void
print_struct_annotation(String& struct_annotation_name, FILE *output, StructAnnotations& struct_annotations, u32 indent)
{
  StructAnnotation *struct_annotation = get_struct_annotation(struct_annotations, struct_annotation_name);

  if (struct_annotation == NULL)
  {
    printf("%.*s", STR_PRINT(struct_annotation_name));
  }
  else
  {
    printf("struct %.*s {\n", STR_PRINT(struct_annotation->type_name));

    for (u32 member_i = 0;
         member_i < struct_annotation->members.n_elements;
         ++member_i)
    {
      StructAnnotationMember& member = struct_annotation->members[member_i];
      printf("%*s", 2*(indent + 1), "");

      print_struct_annotation(member.type_name, output, struct_annotations, indent + 1);

      printf(" %.*s;\n", STR_PRINT(member.label));
    }

    printf("%*s}", 2*indent, "");
  }

  if (indent == 0)
  {
    printf(";\n");
  }
}
