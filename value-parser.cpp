#include "value-parser.h"

#include "parse.h"
#include "maths.h"


b32
is_num(char character)
{
  return (character >= '0' &&
          character <= '9');
}


b32
parse_u32(String *text, u32 *result)
{
  b32 success = true;

  const char *num_start = text->current_position;

  CONSUME_WHILE(*text, is_num);
  u32 num_length = text->current_position - num_start;

  if (num_length == 0)
  {
    success = false;
  }
  else
  {
    *result = 0;

    for (u32 num_pos = 0;
         num_pos < num_length;
         ++num_pos)
    {
      u32 digit = *(num_start + num_pos) - '0';
      *result += digit * pow(10, (num_length - num_pos - 1));
    }
  }

  return success;
}


b32
parse_s32(String *text, s32 *result)
{
  b32 success = true;

  *result = 0;

  b32 coef = 1;
  if (text->current_position != text->end)
  {
    if (*text->current_position == '-')
    {
      ++text->current_position;
      coef = -1;
    }
    else if (*text->current_position == '+')
    {
      ++text->current_position;
      coef = 1;
    }
  }

  const char *num_start = text->current_position;

  CONSUME_WHILE(*text, is_num);

  u32 num_length = text->current_position - num_start;

  if (num_length == 0)
  {
    success = false;
  }
  else
  {
    for (u32 num_pos = 0;
         num_pos < num_length;
         ++num_pos)
    {
      u32 digit = *(num_start + num_pos) - '0';
      *result += digit * pow(10, (num_length - num_pos - 1));
    }

    *result *= coef;
  }

  return success;
}


b32
parse_r32(String *string, r32 *result)
{
  b32 success = true;

  s32 whole_num = 0;
  success &= parse_s32(string, &whole_num);

  if (success)
  {
    *result = whole_num;

    if (string->current_position != string->end && *string->current_position == '.')
    {
      ++string->current_position;

      u32 frac_num = 0;
      success &= parse_u32(string, &frac_num);
      if (!success)
      {
        *result = 0;
      }
      else
      {
        r32 frac_part = frac_num;
        while (frac_part >= 1)
        {
          frac_part /= 10;
        }

        if (whole_num < 0)
        {
          frac_part *= -1;
        }

        *result += frac_part;
      }
    }
  }

  return success;
}