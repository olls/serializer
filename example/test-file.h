#ifndef TEST_FILE_H_DEF
#define TEST_FILE_H_DEF

#include "../libs/types.h"

#include "../serializer/struct-annotator.h"


#define vec2_DEF(MEMBER, n) \
  MEMBER(r32, x, n) \
  MEMBER(r32, y, n)

ANNOTATED_STRUCT(vec2, vec2_DEF)


#define Entity_DEF(M, n) \
  M(vec2, position, n) \
  M(vec2, velocity, n) \
  M(u32, id, n) \
  M(char, letter, n)

ANNOTATED_STRUCT(Entity, Entity_DEF);


#define TestStruct_DEF(MEMBER, n) \
  MEMBER(u32, value_a, n) \
  MEMBER(r32, value_b, n) \
  MEMBER(s32, value_c, n) \
  MEMBER(vec2, position, n) \
  MEMBER(Entity, child, n)

ANNOTATED_STRUCT(TestStruct, TestStruct_DEF);


#endif