#ifndef TEST_FILE_H_DEF
#define TEST_FILE_H_DEF

#include "../libs/types.h"

#include "../serializer/struct-annotator.h"


ANNOTATED_TYPEDEF(r32, real);


#define vec2_DEF(MEMBER, MEMBER_A, n) \
  MEMBER(real, x, n) \
  MEMBER(real, y, n)

ANNOTATED_STRUCT(vec2, vec2_DEF);

ANNOTATED_TYPEDEF(vec2, MyAwesomeVector);

#define Entity_DEF(M, M_A, n) \
  M_A(MyAwesomeVector, position, 2, n) \
  M(vec2, velocity, n) \
  M(u32, id, n) \
  M_A(char, letter, 4, n)

ANNOTATED_STRUCT(Entity, Entity_DEF);


#define TestStruct_DEF(MEMBER, MEMBER_A, n) \
  MEMBER(u32, value_a, n) \
  MEMBER(r32, value_b, n) \
  MEMBER(s32, value_c, n) \
  MEMBER(vec2, position, n) \
  MEMBER(Entity, child, n)

ANNOTATED_STRUCT(TestStruct, TestStruct_DEF);


#endif