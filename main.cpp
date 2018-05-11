#include "types.h"
#include "files.h"
#include "string.h"
#include "struct-annotator.h"
#include "serializer.h"
#include "deserializer.h"

#include "test-file.h"

#include <stdio.h>


s32
main(s32 argc, char const *argv[])
{
  b32 success = true;

  File file = {};
  success = open_file("test-file.ini", &file);

  StructAnnotation test_struct_annotation = get_annotated_TestStruct();

  TestStruct test_struct = {};

  if (success)
  {
    String file_text = {
      .start = file.read_ptr,
      .current_position = file.read_ptr,
      .end = file.read_ptr + file.size
    };

    printf("\n\n## Deserialize ini file\n\n");

    success &= deserialize_struct(file_text, test_struct_annotation, &test_struct);
    if (success)
    {
      print_annotated_struct(test_struct_annotation, &test_struct);
    }
  }

  close_file(&file);

  test_struct.value_a += 1;
  test_struct.value_b *= 1.01;

  printf("\n\n## Serialize to ini file\n\n");

  Array::Array<char> output = {};
  serialize_struct(output, test_struct_annotation, &test_struct);

  success = open_file("test-file.ini", &file, true, output.n_elements);
  if (success)
  {
    memcpy(file.write_ptr, output.elements, output.n_elements);
  }

  close_file(&file);
  Array::free_array(output);

  if (success)
  {
    printf("Done\n");
    return 0;
  }
  else
  {
    printf("Error\n");
    return 1;
  }
}