#include "struct-annotator.h"

#include "serializable-types.h"

#include <stdio.h>


StructAnnotation *
get_struct_annotation(StructAnnotations& struct_annotations, const char *annotation_name)
{
  return Hashmap::get(struct_annotations.map, annotation_name, strlen(annotation_name));
}


void
print_struct_annotation(const char *struct_annotation_name, FILE *output, StructAnnotations& struct_annotations, u32 indent)
{
  StructAnnotation *struct_annotation = get_struct_annotation(struct_annotations, struct_annotation_name);

  const char *aliased_type_name = struct_annotation_name;
  const char *type_name_alias = struct_annotation_name;
  while (struct_annotation != NULL &&
         struct_annotation->type_alias)
  {
    aliased_type_name = struct_annotation->aliased_type;
    struct_annotation = get_struct_annotation(struct_annotations, aliased_type_name);
  }

  if (struct_annotation != NULL)
  {
    printf("struct ");
  }

  if (aliased_type_name == type_name_alias)
  {
    printf("%s", type_name_alias);
  }
  else
  {
    printf("(%s -> %s)", type_name_alias, aliased_type_name);
  }

  if (struct_annotation != NULL)
  {
    printf(" {\n");

    for (u32 member_i = 0;
         member_i < struct_annotation->members.n_elements;
         ++member_i)
    {
      StructAnnotationMember& member = struct_annotation->members[member_i];
      printf("%*s", 2*(indent + 1), "");

      print_struct_annotation(member.type_name, output, struct_annotations, indent + 1);

      printf(" %s", member.label);
      if (member.array_size > 1)
      {
        printf("[%d]", member.array_size);
      }
      printf(";\n");
    }

    printf("%*s}", 2*indent, "");
  }

  if (indent == 0)
  {
    printf(";\n");
  }
}
