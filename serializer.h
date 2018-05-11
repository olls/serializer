#ifndef SERIALIZER_H_DEF
#define SERIALIZER_H_DEF

#include "string.h"
#include "struct-annotator.h"


void
serialize_struct(Array::Array<char>& output, StructAnnotation& annotated_struct, void *data);


#endif