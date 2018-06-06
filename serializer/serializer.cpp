#include "serializer.h"

#include "serializable-types.h"
#include <stdio.h>


void
serialize_data(const char *type_name, const char *label, void *data, FILE *output, u32 array_size, StructAnnotations& struct_annotations, u32 indent)
{
  StructAnnotation *struct_annotation = get_struct_annotation(struct_annotations, type_name);
  const char *aliased_type = type_name;

  while (struct_annotation != NULL &&
         struct_annotation->type_alias)
  {
    aliased_type = struct_annotation->aliased_type;
    struct_annotation = get_struct_annotation(struct_annotations, aliased_type);
  }

  u32 element_size;
  SerializableType no_annotation_type;
  if (struct_annotation == NULL)
  {
    if (serializable_type(aliased_type, &no_annotation_type))
    {
      element_size = serializable_type_size(no_annotation_type);
    }
    else
    {
      element_size = 0;
    }
  }
  else
  {
    element_size = struct_annotation->size;
  }

  fprintf(output, "%*s%s %s", 2 * indent, "", type_name, label);
  if (array_size > 1)
  {
    fprintf(output, "[%d]", array_size);
  }
  fprintf(output, " = ");

  if (array_size > 1)
  {
    fprintf(output, "[");
  }

  for (u32 array_element_index = 0;
       array_element_index < array_size;
       ++array_element_index)
  {
    void *array_element_data = (u8*)data + (array_element_index * element_size);

    if (struct_annotation == NULL)
    {
      if (element_size > 0)
      {
        serialize_type(no_annotation_type, array_element_data, output);
      }
      else
      {
        fprintf(output, "[No %s annotation]", type_name);
      }
    }
    else
    {
      fprintf(output, "struct %s {\n", type_name);

      for (u32 member_i = 0;
           member_i < struct_annotation->members.n_elements;
           ++member_i)
      {
        StructAnnotationMember& member = struct_annotation->members[member_i];

        void *member_data = (u8*)array_element_data + member.offset;

        serialize_data(member.type_name, member.label, member_data, output, member.array_size, struct_annotations, indent + 1);
        fprintf(output, ";\n");
      }
      fprintf(output, "%*s}", 2*indent, "");
    }

    if (array_size > 1 &&
        array_element_index != array_size - 1)
    {
      fprintf(output, ", ");
    }
  }

  if (array_size > 1)
  {
    fprintf(output, "]");
  }

  if (indent == 0)
  {
    fprintf(output, ";\n");
  }
}
