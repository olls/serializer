#ifndef SERIALIZABLE_TYPES_H_DEF
#define SERIALIZABLE_TYPES_H_DEF

#include "../libs/types.h"
#include <stdio.h>


enum SerializableType
{
  SerializableType__uint32_t,
  SerializableType__int32_t,
  SerializableType__float,
  SerializableType__char
};


b32
serializable_type(const char *type_name, SerializableType *result);


void
serialize_type(SerializableType type, void *data, FILE *output);


u32
serializable_type_size(SerializableType type);


#endif