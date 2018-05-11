#ifndef SERIALIZABLE_TYPES_H_DEF
#define SERIALIZABLE_TYPES_H_DEF


enum struct SerializableType
{
  u32,
  s32,
  r32,
  vec2
};


static const char *TYPE_STRINGS[] = {
  "u32",
  "s32",
  "r32",
  "vec2"
};


#endif