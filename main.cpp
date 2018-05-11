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
  printf("\n\n## Test struct annotation\n\n");

  StructAnnotation test_struct_annotation = get_annotated_TestStruct();

  TestStruct test_struct_data = {
    1, 1.4, {1, 21.3}
  };

  Array::Array<char> output = {};
  serialize_struct(output, test_struct_annotation, &test_struct_data);
  printf("%.*s\n", output.n_elements, output.elements);

  printf("\n\n## Test deserialize ini file\n\n");

  File file = {};
  b32 success = open_file("test-file.ini", &file);

  if (success)
  {
    String file_text = {
      .start = file.read_ptr,
      .current_position = file.read_ptr,
      .end = file.read_ptr + file.size
    };

    TestStruct *result = (TestStruct*)deserialize_struct(file_text, test_struct_annotation);
    print_annotated_struct(test_struct_annotation, result);
    free(result);

    // String test_struct_section = find_section(file_text, "TestStruct");
    // if (test_struct_section.start == 0)
    // {
    //   printf("Could not find section TestStruct\n");
    //   success = false;
    // }
    // else
    // {
    //   String value_a_line = find_line_starting_with(test_struct_section, "value_a");
    //   if (value_a_line.start == 0)
    //   {
    //     printf("Could not find value_a in TestStruct section\n");
    //     success = false;
    //   }
    //   else
    //   {
    //     printf("\"%.*s\"\n", STR_PRINT(value_a_line));
    //   }
    // }
  }

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