#ifndef DESERIALIZER_H_DEF
#define DESERIALIZER_H_DEF

#include "string.h"
#include "struct-annotator.h"
#include "fcpp-lexer-1.1/4cpp_lexer_types.h"


b32
deserialize_value(String text, String type_name, String label, void *result, StructAnnotations& struct_annotations = global_struct_annotations);


#endif