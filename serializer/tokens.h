#ifndef TOKENS_H_DEF
#define TOKENS_H_DEF

#include <stdio.h>

#include "struct-annotator.h"
#include "../libs/fcpp-lexer-1.1/4cpp_lexer.h"


#define TokenRange_DEF(M, M_A, n) \
  M(s32, start_token, n) \
  M(s32, end_token, n) // Index of token outside the range

ANNOTATED_STRUCT(TokenRange, TokenRange_DEF);


static b32
token_sequence_matches(CppLexer::Cpp_Token_Array tokens, TokenRange token_range, s32 sequence_length, CppLexer::Cpp_Token_Type *token_sequence)
{
  b32 result = true;

  if (token_range.start_token + sequence_length > token_range.end_token)
  {
    result = false;
  }
  else
  {
    for (s32 token_offset = 0;
         token_offset < sequence_length;
         ++token_offset)
    {
      CppLexer::Cpp_Token token = tokens.tokens[token_range.start_token + token_offset];

      if (token.type != token_sequence[token_offset])
      {
        result = false;
        break;
      }
    }
  }

  return result;
}


inline String
token_string(String text, CppLexer::Cpp_Token token)
{
  return (String){
    .start = text.start + token.start,
    .current_position = text.start + token.start,
    .end = text.start + token.start + token.size
  };
}


inline void
print_token_range_string(String text, CppLexer::Cpp_Token_Array tokens, TokenRange token_range)
{
  for (s32 token_index = token_range.start_token;
       token_index < token_range.end_token;
       ++token_index)
  {
    printf("%.*s ", STR_PRINT(token_string(text, tokens.tokens[token_index])));
  }
  printf("\n");
}


inline void
print_token_range(CppLexer::Cpp_Token_Array tokens, TokenRange token_range)
{
  for (s32 token_index = token_range.start_token;
       token_index < token_range.end_token;
       ++token_index)
  {
    printf("%s  ", CppLexer::cpp_token_string(tokens.tokens[token_index]));
  }
  printf("\n");
}


// Returns the position of the first token found at the same scope level as token_range.start_token.  Assuming the first
//   token in the token_range is a start_scope result will be the next token past the matching end_scope.  (If the first
//   token is not start_scope then the result will be equal to token_range.start_token)
//
static s32
find_end_scope_token(CppLexer::Cpp_Token_Array tokens, TokenRange token_range, CppLexer::Cpp_Token_Type start_scope, CppLexer::Cpp_Token_Type end_scope)
{
  s32 result = -1;

  u32 open_braces_count = 0;
  u32 close_braces_count = 0;

  for (s32 token_index = token_range.start_token;
       token_index < token_range.end_token;
       ++token_index)
  {
    CppLexer::Cpp_Token_Type current_token_type = tokens.tokens[token_index].type;

    b32 found_start_scope = current_token_type == start_scope;
    b32 found_end_scope = current_token_type == end_scope;

    if (found_start_scope)
    {
      open_braces_count += 1;
    }
    if (found_end_scope)
    {
      close_braces_count += 1;
    }

    if (found_end_scope &&
        open_braces_count == close_braces_count)
    {
      result = token_index;
      break;
    }
  }

  return result;
}


static s32
skip_scoped_sections(CppLexer::Cpp_Token_Array tokens, s32 token_index, s32 end_token)
{
  char error;

  if (tokens.tokens[token_index].type == CppLexer::CPP_TOKEN_BRACE_OPEN)
  {
    error = '}';
    token_index = find_end_scope_token(tokens, {token_index, end_token},
                                       CppLexer::CPP_TOKEN_BRACE_OPEN,
                                       CppLexer::CPP_TOKEN_BRACE_CLOSE);
  }
  else
  if (tokens.tokens[token_index].type == CppLexer::CPP_TOKEN_BRACKET_OPEN)
  {
    error = ']';
    token_index = find_end_scope_token(tokens, {token_index, end_token},
                                       CppLexer::CPP_TOKEN_BRACKET_OPEN,
                                       CppLexer::CPP_TOKEN_BRACKET_CLOSE);
  }

  if (token_index == -1)
  {
    printf("No matching %c found.\n", error);
  }

  return token_index;
}


static TokenRange
find_next_token_at_matching_scope(CppLexer::Cpp_Token_Array tokens, TokenRange token_range, CppLexer::Cpp_Token_Type token_type)
{
  TokenRange result = {-1};
  s32 token_index = token_range.start_token;

  while (token_index < token_range.end_token)
  {
    token_index = skip_scoped_sections(tokens, token_index, token_range.end_token);
    if (token_index == -1)
    {
      break;
    }

    if (tokens.tokens[token_index].type == token_type)
    {
      result.start_token = token_range.start_token;
      result.end_token = token_index;
      break;
    }

    token_index += 1;
  }

  return result;
}


#endif