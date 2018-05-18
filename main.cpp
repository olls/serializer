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
  print_struct_annotation(TestStruct_annotation_type_name, stdout);
  printf("\n");

  TestStruct test_struct = {};
  String test_struct_label= STRING("test_struct");

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

    success &= deserialize_value(file_text, TestStruct_annotation_type_name, test_struct_label, &test_struct);
    if (success)
    {
      serialize_data(TestStruct_annotation_type_name, STRING("deserialized_test_struct"), &test_struct, stdout);
    }
  }

  close_file(&file);

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
    serialize_data(TestStruct_annotation_type_name, STRING("test_struct"), &test_struct, output);
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