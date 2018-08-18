#include "../libs/types.h"
#include "../libs/files.h"
#include "../libs/string.h"

#include "../serializer/struct-annotator.h"
#include "../serializer/serializer.h"
#include "../serializer/deserializer.h"
#include "../serializer/ast.h"

#include "test-file.h"

#include <stdio.h>


s32
main(s32 argc, char const *argv[])
{
  b32 success = true;

  const char file_name[] = "example/test-file.dat";

  printf("\n# Struct Annotation\n\n");
  Hashmap::print_stats(global_struct_annotations.map);
  print_struct_annotation(MyAwesomeVector__annotation_type_name);
  print_struct_annotation(TestStruct__annotation_type_name);
  printf("\n");

  TestStruct test_struct = {};

  u32 meaning_of_life = 0;

  printf("\n# Deserialize data file\n\n");

  File file = {};
  success &= open_file(file_name, &file);
  if (success)
  {
    String file_text = {
      .start = file.read_ptr,
      .end = file.read_ptr + file.size,
      .current_position = file.read_ptr
    };

    b32 test_struct_deserialized = deserialize_value(file_text, TestStruct__annotation_type_name, "test_struct", &test_struct);
    if (test_struct_deserialized)
    {
      serialize_data(TestStruct__annotation_type_name, "deserialized_test_struct", &test_struct);
    }
    printf("\n");

    b32 meaning_of_life_deserialized = deserialize_value(file_text, "u32", "meaning_of_life", &meaning_of_life);
    if (meaning_of_life_deserialized)
    {
      serialize_data("u32", "deserialized_meaning_of_life", &meaning_of_life);
    }
    printf("\n");

    success &= test_struct_deserialized;
    success &= meaning_of_life_deserialized;
  }

  close_file(&file);

  printf("\n# Serialize to data file\n\n");

  test_struct.value_a += 1;
  test_struct.value_b *= 1.01;

  FILE *output = fopen(file_name, "w");
  if (output == NULL)
  {
    printf("Failed to open %s\n", file_name);
    success = false;
  }
  else
  {
    serialize_data(TestStruct__annotation_type_name, "test_struct", &test_struct, output);
    serialize_data("u32", "meaning_of_life", &meaning_of_life, output);
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