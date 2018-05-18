#include "serializer.h"

#include "string.h"
#include "serializable-types.h"

#include <stdio.h>


void
serialize_struct(String label, String& struct_name, void *data, FILE *output, StructAnnotations& struct_annotations, u32 indent)
{
  StructAnnotation *struct_annotation = get_struct_annotation(struct_annotations, struct_name);

  if (struct_annotation == NULL)
  {
    fprintf(output, "%*s%.*s %.*s = ", 2 * indent, "", STR_PRINT(struct_name), STR_PRINT(label));

    b32 serialized = serialize_type(struct_name, data, output);

    if (!serialized)
    {
      fprintf(output, "[No %.*s annotation]", STR_PRINT(struct_name));
    }
  }
  else
  {
    fprintf(output, "%*s%.*s %.*s = struct %.*s {\n", 2 * indent, "", STR_PRINT(struct_annotation->type_name), STR_PRINT(label), STR_PRINT(struct_annotation->type_name));

    for (u32 member_i = 0;
         member_i < struct_annotation->members.n_elements;
         ++member_i)
    {
      StructAnnotationMember& member = struct_annotation->members[member_i];

      void *member_data = (u8*)data + member.offset;

      serialize_struct(member.label, member.type_name, member_data, output, struct_annotations, indent + 1);
      fprintf(output, ";\n");
    }
    fprintf(output, "%*s}", 2*indent, "");

  }

  if (indent == 0)
  {
    fprintf(output, ";\n");
  }
}
