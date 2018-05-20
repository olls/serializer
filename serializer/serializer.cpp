#include "serializer.h"

#include "string.h"
#include "serializable-types.h"

#include <stdio.h>


void
serialize_data(String type_name, String label, void *data, FILE *output, StructAnnotations& struct_annotations, u32 indent)
{
  StructAnnotation *struct_annotation = get_struct_annotation(struct_annotations, type_name);

  fprintf(output, "%*s%.*s %.*s = ", 2 * indent, "", STR_PRINT(type_name), STR_PRINT(label));

  String serializable_type_name = type_name;
  if (struct_annotation != NULL &&
      struct_annotation->type_alias)
  {
    serializable_type_name = struct_annotation->aliased_type;
    struct_annotation = NULL;
  }

  if (struct_annotation == NULL)
  {
    b32 serialized = serialize_type(serializable_type_name, data, output);

    if (!serialized)
    {
      fprintf(output, "[No %.*s annotation]", STR_PRINT(type_name));
    }
  }
  else
  {
    fprintf(output, "struct %.*s {\n", STR_PRINT(struct_annotation->type_name));

    for (u32 member_i = 0;
         member_i < struct_annotation->members.n_elements;
         ++member_i)
    {
      StructAnnotationMember& member = struct_annotation->members[member_i];

      void *member_data = (u8*)data + member.offset;

      serialize_data(member.type_name, member.label, member_data, output, struct_annotations, indent + 1);
      fprintf(output, ";\n");
    }
    fprintf(output, "%*s}", 2*indent, "");
  }

  if (indent == 0)
  {
    fprintf(output, ";\n");
  }
}
