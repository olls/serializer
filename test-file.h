#ifndef TEST_FILE_H_DEF
#define TEST_FILE_H_DEF

#include "types.h"

#include "struct-annotator.h"


#define TestStruct_DEF(MEMBER, n) \
  MEMBER(u32, value_a, n) \
  MEMBER(r32, value_b, n) \
  MEMBER(s32, value_c, n) \
  MEMBER(vec2, position, n)

ANNOTATED_STRUCT(TestStruct, TestStruct_DEF);


#endif