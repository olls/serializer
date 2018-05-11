#include "serializer.h"

#include "test-file.h"


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
  for (u32 member_i = 0;
       member_i < annotated_struct.members.n_elements;
       ++member_i)

  {
    StructAnnotationMember& member_annotation = annotated_struct.members[member_i];

    void *member_data = (u8 *)data + member_annotation.offset;
    // printf("serializing %s %.*s\n", TYPE_STRINGS[u32(member_annotation.type)], STR_PRINT(member_annotation.name));

    const u32 buffer_size = 1024;
    char buffer[buffer_size];
    WriteString line = {
      .start = buffer,
      .end = buffer + buffer_size,
      .current_position = buffer
    };


    append_string(line, member_annotation.name);
    append_string(line, ": ");
    serialize_value(&line, member_annotation.type, member_data);
    append_string(line, "\n");

    Array::add_n(output, line.start, line.current_position - line.start);
  }
}