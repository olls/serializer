#include "value-parser.h"

#include "../libs/parse.h"
#include <math.h>


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

  s32 coef = 1;
  if (string->current_position != string->end)
  {
    if (*string->current_position == '-')
    {
      ++string->current_position;
      coef = -1;
    }
    else if (*string->current_position == '+')
    {
      ++string->current_position;
      coef = 1;
    }
  }

  const char *num_start = string->current_position;

  CONSUME_WHILE(*string, is_num);

  u32 num_length = string->current_position - num_start;

  s32 whole_num = 0;

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
      whole_num += digit * pow(10, (num_length - num_pos - 1));
    }
  }

  if (success)
  {
    *result = whole_num;

    if (string->current_position != string->end && *string->current_position == '.')
    {
      ++string->current_position;

      r32 frac_num = 0;

      const char *frac_start = string->current_position;

      CONSUME_WHILE(*string, is_num);
      u32 frac_length = string->current_position - frac_start;

      if (frac_length == 0)
      {
        success = false;
        *result = 0;
      }
      else
      {
        for (u32 num_pos = 0;
             num_pos < frac_length;
             ++num_pos)
        {
          u32 digit = *(frac_start + num_pos) - '0';
          frac_num += digit / pow(10, num_pos + 1);
        }

        *result += frac_num;
      }
    }

  }

  *result = (*result) * coef;

  return success;
}


b32
parse_char(String *text, char *result)
{
  b32 success = true;

  if (text->current_position[0] == '\'')
  {
    text->current_position += 1;

    if (text->current_position >= text->end)
    {
      success = false;
    }
    else
    {
      if (text->current_position[0] == '\\')
      {
        text->current_position += 1;
        if (text->current_position >= text->end)
        {
          success = false;
        }
        else
        {
          switch (text->current_position[0])
          {
            case ('\\'):
              *result = '\\';
              break;
            case ('n'):
              *result = '\n';
              break;
            case ('t'):
              *result = '\t';
              break;
            case ('\''):
              *result = '\'';
              break;
            default:
              success = false;
          }
          text->current_position += 1;
        }
      }
      else
      {
        *result = text->current_position[0];
        text->current_position += 1;
      }
    }

    if (text->current_position >= text->end ||
        text->current_position[0] != '\'')
    {
      success = false;
    }
    else
    {
      text->current_position += 1;
    }
  }
  else
  {
    u32 integer_constant = 0;
    success = parse_u32(text, &integer_constant);
    if (success)
    {
      *result = integer_constant;
    }
  }

  return success;
}