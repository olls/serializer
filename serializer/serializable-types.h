#ifndef SERIALIZABLE_TYPES_H_DEF
#define SERIALIZABLE_TYPES_H_DEF

#include "../libs/string.h"

#include <stdio.h>


b32
serialize_type(String& type_name, void *data, FILE *output);


#endif