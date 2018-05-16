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

  printf("\n# Struct Annotation\n\n");
  Hashmap::print_stats(global_struct_annotations.map);
  print_struct_annotation(TestStruct_annotation_name, stdout);
  printf("\n");

  TestStruct test_struct = {};

  printf("\n# Deserialize data file\n\n");

  File file = {};
  success &= open_file("test-file.dat", &file);
  if (success)
  {
    String file_text = {
      .start = file.read_ptr,
      .end = file.read_ptr + file.size,
      .current_position = file.read_ptr
    };

    // success &= deserialize_struct(file_text, TestStruct_annotation_name, &test_struct);
    if (success)
    {
      serialize_struct(STRING("test_struct"), TestStruct_annotation_name, &test_struct, stdout);
    }
  }

  close_file(&file);

  printf("\n# Serialize to stdout\n\n");
  serialize_struct(STRING("test_struct"), TestStruct_annotation_name, &test_struct, stdout);
  printf("\n");

  printf("\n# Serialize to data file\n\n");

  test_struct.value_a += 1;
  test_struct.value_b *= 1.01;

  FILE *output = fopen("test-file.dat", "w");
  if (output == NULL)
  {
    printf("Failed to open test-file.dat\n");
    success = false;
  }
  else
  {
    serialize_struct(STRING("test_struct"), TestStruct_annotation_name, &test_struct, output);
  }
  fclose(output);

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