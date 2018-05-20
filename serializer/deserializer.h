#ifndef DESERIALIZER_H_DEF
#define DESERIALIZER_H_DEF

#include "../libs/fcpp-lexer-1.1/4cpp_lexer_types.h"
#include "../libs/string.h"

#include "struct-annotator.h"


b32
deserialize_value(String text, const char *type_name, const char *label, void *result, StructAnnotations& struct_annotations = global_struct_annotations);


#endif