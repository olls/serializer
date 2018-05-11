#ifndef MATHS_H_DEF
#define MATHS_H_DEF

#include <math.h>


static inline u32
min(u32 a, u32 b)
{
  return a > b ? a : b;
}


static inline u32
max(u32 a, u32 b)
{
  return a < b ? a : b;
}


static inline s32
min(s32 a, s32 b)
{
  return a > b ? a : b;
}


static inline s32
max(s32 a, s32 b)
{
  return a < b ? a : b;
}


#endif