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
  File file = {};
  b32 success = open_file("test-file.ini", &file);

  StructAnnotation test_struct_annotation = get_annotated_TestStruct();

  if (success)
  {
    String file_text = {
      .start = file.read_ptr,
      .current_position = file.read_ptr,
      .end = file.read_ptr + file.size
    };

    printf("\n\n## Deserialize ini file\n\n");

    TestStruct *test_struct = (TestStruct*)deserialize_struct(file_text, test_struct_annotation);
    if (test_struct == 0)
    {
      success = false;
    }
    else
    {
      print_annotated_struct(test_struct_annotation, test_struct);

      printf("\n\n## Serialize to console\n\n");

      Array::Array<char> output = {};
      serialize_struct(output, test_struct_annotation, test_struct);
      printf("%.*s\n", output.n_elements, output.elements);

      free(test_struct);
    }
  }

  close_file(&file);

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