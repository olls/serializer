#ifndef SERIALIZABLE_TYPES_H_DEF
#define SERIALIZABLE_TYPES_H_DEF

#include "../libs/types.h"
#include <stdio.h>


b32
serialize_type(const char *type_name, void *data, FILE *output);


#endif