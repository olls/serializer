#ifndef DESERIALIZER_H_DEF
#define DESERIALIZER_H_DEF

#include "string.h"
#include "struct-annotator.h"


void *
deserialize_struct(String text, StructAnnotation& struct_annotation);


#endif