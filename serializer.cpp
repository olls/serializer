#include "serializer.h"

#include "test-file.h"
#include "array-string.h"
#include "string.h"


// TODO: global hashtable storage for StructAnnotation's
//       This will enable recursive struct serialization


// TODO: create a wrapper for snprintf which writes to Array<char>


void
serialize_value(WriteString *output, SerializableType type, void *data)
{
  b32 success = true;

  s32 characters_written = 0;

  switch (type)
  {
    case (SerializableType::u32):
    {
      characters_written = snprintf(output->current_position, SPACE_LEFT(*output), "%u", *(u32 *)data);
    } break;
    case (SerializableType::s32):
    {
      characters_written = snprintf(output->current_position, SPACE_LEFT(*output), "%d", *(s32 *)data);
    } break;
    case (SerializableType::r32):
    {
      characters_written = snprintf(output->current_position, SPACE_LEFT(*output), "%f", *(r32 *)data);
    } break;
    case (SerializableType::vec2):
    {
      characters_written = snprintf(output->current_position, SPACE_LEFT(*output), "%f %f", ((vec2*)data)->x, ((vec2*)data)->y);
    } break;

    default:
    {
      char unimplemented[] = "[unimplemented type for serialization]";
      strcpy(output->current_position, unimplemented);
      characters_written = strlen(unimplemented);
    }
  }

  output->current_position += characters_written;
  if (output->current_position > output->end)
  {
    output->current_position = output->end;
    printf("Out of space in serialization buffer\n");
  }
}


void
serialize_struct(Array::Array<char>& output, StructAnnotation& annotated_struct, void *data)
{
  append_string(output, "[");
  append_string(output, annotated_struct.name);
  append_string(output, "]\n");

  const u32 buffer_size = 1024;
  char buffer[buffer_size];

  WriteString value_buffer = {
    .start = buffer,
    .end = buffer + buffer_size
  };
  for (u32 member_i = 0;
       member_i < annotated_struct.members.n_elements;
       ++member_i)

  {
    StructAnnotationMember& member_annotation = annotated_struct.members[member_i];

    void *member_data = (u8 *)data + member_annotation.offset;

    append_string(output, member_annotation.name);
    append_string(output, ": ");

    value_buffer.current_position = value_buffer.start;
    serialize_value(&value_buffer, member_annotation.type, member_data);

    Array::add_n(output, value_buffer.start, value_buffer.current_position - value_buffer.start);
    append_string(output, "\n");
  }
  append_string(output, "\n");
}