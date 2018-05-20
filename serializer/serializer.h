#ifndef SERIALIZER_H_DEF
#define SERIALIZER_H_DEF

#include "../libs/types.h"

#include "struct-annotator.h"


void
serialize_data(const char *type_name, const char *label, void *data, FILE *output = stdout, StructAnnotations& struct_annotations = global_struct_annotations, u32 indent = 0);


#endif