#ifndef DESERIALIZER_H_DEF
#define DESERIALIZER_H_DEF

#include "string.h"
#include "struct-annotator.h"


b32
deserialize_struct(String text, StructAnnotation& struct_annotation, void *result);


#endif