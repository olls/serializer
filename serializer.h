#ifndef SERIALIZER_H_DEF
#define SERIALIZER_H_DEF

#include "types.h"
#include "string.h"
#include "struct-annotator.h"


void
serialize_struct(String label, String& struct_name, void *data, FILE *output = stdout, StructAnnotations& struct_annotations = global_struct_annotations, u32 indent = 0);


#endif