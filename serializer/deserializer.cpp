#include "deserializer.h"

#include "value-parser.h"
#include "serializable-types.h"
#include "parse.h"

#include "../libs/fcpp-lexer-1.1/4cpp_lexer.h"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))


struct TokenArray
{
  CppLexer::Cpp_Token *tokens;
  s32 count;
};


struct TokenArrayRange
{
  s32 start_token;
  s32 end_token; // Index of token outside the range
};


b32
token_sequence_matches(TokenArray tokens, TokenArrayRange token_range, s32 sequence_length, CppLexer::Cpp_Token_Type *token_sequence)
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


// Returns the position of the first token found at the same scope level as token_range.start_token.  Assuming the first
//   token in token range is an "{" result will be the next token past the matching "}".  (If the first token is not "{"
//   then the result will be equal to token_range.start_token)
//
s32
find_matching_brace_token(TokenArray tokens, TokenArrayRange token_range)
{
  s32 result = -1;

  u32 open_braces_count = 0;
  u32 close_braces_count = 0;

  for (s32 token_index = token_range.start_token;
       token_index < token_range.end_token;
       ++token_index)
  {
    CppLexer::Cpp_Token_Type current_token_type = tokens.tokens[token_index].type;

    b32 found_open_brace = current_token_type == CppLexer::CPP_TOKEN_BRACE_OPEN;
    b32 found_close_brace = current_token_type == CppLexer::CPP_TOKEN_BRACE_CLOSE;

    if (found_open_brace)
    {
      open_braces_count += 1;
    }
    if (found_close_brace)
    {
      close_braces_count += 1;
    }

    if (found_close_brace &&
        open_braces_count == close_braces_count)
    {
      result = token_index;
      break;
    }
  }

  return result;
}


// Searches the token array for a matching 'type label =' at the current level scope of the passed in tokens, returns
//   the token range of its value.
//
TokenArrayRange
find_value_in_tokens(String text, TokenArray tokens, TokenArrayRange token_range, String type_name, String label)
{
  TokenArrayRange result = {-1};

  assert(token_range.start_token >= 0);
  assert(token_range.end_token <= tokens.count);

  // Search tokens for a matching "type_name label = ", skipping any braced sections to stay at the top level scope.
  //   Once found, return all tokens from label to the next ";" which is at __the same scope level as the label__.

  // Need two sequences because the type can be either KEY_TYPE, or IDENTIFIER depending on whether it is an standard or
  //   user defined type...
  CppLexer::Cpp_Token_Type type_ident_seq_a[] = {
    CppLexer::CPP_TOKEN_KEY_TYPE,
    CppLexer::CPP_TOKEN_IDENTIFIER,
    CppLexer::CPP_TOKEN_EQ
  };
  CppLexer::Cpp_Token_Type type_ident_seq_b[] = {
    CppLexer::CPP_TOKEN_IDENTIFIER,
    CppLexer::CPP_TOKEN_IDENTIFIER,
    CppLexer::CPP_TOKEN_EQ
  };

  u32 sequence_length = ARRAY_SIZE(type_ident_seq_a);
  assert(sequence_length == ARRAY_SIZE(type_ident_seq_b));

  for (s32 token_index = token_range.start_token;
       token_index < token_range.end_token;
       ++token_index)
  {
    // Skip any braced sections

    if (tokens.tokens[token_index].type == CppLexer::CPP_TOKEN_BRACE_OPEN)
    {
      token_index = find_matching_brace_token(tokens, {token_index, token_range.end_token});
      if (token_index == -1)
      {
        printf("No matching {} found.\n");
        break;
      }
    }
    else
    if ((token_sequence_matches(tokens, {token_index, token_range.end_token}, sequence_length, type_ident_seq_a) ||
         token_sequence_matches(tokens, {token_index, token_range.end_token}, sequence_length, type_ident_seq_b)) &&
        string_eq(token_string(text, tokens.tokens[token_index + 0]), type_name) &&
        string_eq(token_string(text, tokens.tokens[token_index + 1]), label))
    {
      // Found: 'IDENTIFIER IDENTIFIER ='
      token_index += sequence_length;

      // Found start of the matching value
      result.start_token = token_index;
      break;
    }

    if (token_index >= token_range.end_token)
    {
      printf("Ran out of tokens whilst parsing value.\n");
      break;
    }
  }

  // Find the end of the value tokens
  if (result.start_token != -1)
  {
    for (s32 token_index = result.start_token;
         token_index < token_range.end_token;
         ++token_index)
    {
      // Skip any braced section
      if (tokens.tokens[token_index].type == CppLexer::CPP_TOKEN_BRACE_OPEN)
      {
        token_index = find_matching_brace_token(tokens, {token_index, token_range.end_token});
        if (token_index == -1)
        {
          printf("No matching {} found.\n");
          result.start_token = -1;
          break;
        }
      }
      else
      if (tokens.tokens[token_index].type == CppLexer::CPP_TOKEN_SEMICOLON)
      {
        // Found end of value
        result.end_token = token_index;
        break;
      }
    }
  }

  return result;
}


TokenArrayRange
get_struct_contents(String text, TokenArray tokens, TokenArrayRange token_range, String type_name)
{
  TokenArrayRange result = {-1};

  CppLexer::Cpp_Token_Type struct_decl_seq[] = {
    CppLexer::CPP_TOKEN_KEY_TYPE_DECLARATION,
    CppLexer::CPP_TOKEN_IDENTIFIER,
    CppLexer::CPP_TOKEN_BRACE_OPEN
  };

  if (token_sequence_matches(tokens, token_range, ARRAY_SIZE(struct_decl_seq), struct_decl_seq) &&
      string_eq(token_string(text, tokens.tokens[token_range.start_token + 1]), type_name))
  {
    // Found: 'struct type_name {'
    result.start_token = token_range.start_token + ARRAY_SIZE(struct_decl_seq);
    result.end_token = find_matching_brace_token(tokens, {result.start_token-1, token_range.end_token});
    if (result.end_token == -1)
    {
      result.start_token = -1;
      printf("No matching {} found.\n");
    }
  }

  return result;
}


void
print_token_range_string(String text, TokenArray tokens, TokenArrayRange token_range)
{
  for (s32 token_index = token_range.start_token;
       token_index < token_range.end_token;
       ++token_index)
  {
    printf("%.*s ", STR_PRINT(token_string(text, tokens.tokens[token_index])));
  }
  printf("\n");
}


b32
parse_atomic_type_tokens(String text, TokenArray tokens, TokenArrayRange token_range, String type_name, void *result)
{
  b32 success = true;

  // TODO:  This could be rewritten to be more extensible and use the tokens for parsing the values more nicely

  String token_text = {};
  token_text.start = text.start + tokens.tokens[token_range.start_token].start;
  token_text.end = text.start + tokens.tokens[token_range.end_token].start;
  token_text.current_position = token_text.start;

  assert(token_text.start <= text.end);
  assert(token_text.end <= text.end);

  static CppLexer::Cpp_Token_Type u32_sequence[] = {
    CppLexer::CPP_TOKEN_INTEGER_CONSTANT
  };

  static CppLexer::Cpp_Token_Type s32_sequence_a[] = {
    CppLexer::CPP_TOKEN_INTEGER_CONSTANT
  };
  static CppLexer::Cpp_Token_Type s32_sequence_b[] = {
    CppLexer::CPP_TOKEN_MINUS,
    CppLexer::CPP_TOKEN_INTEGER_CONSTANT
  };
  static CppLexer::Cpp_Token_Type s32_sequence_c[] = {
    CppLexer::CPP_TOKEN_PLUS,
    CppLexer::CPP_TOKEN_INTEGER_CONSTANT
  };

  static CppLexer::Cpp_Token_Type r32_sequence_a[] = {
    CppLexer::CPP_TOKEN_FLOATING_CONSTANT
  };
  static CppLexer::Cpp_Token_Type r32_sequence_b[] = {
    CppLexer::CPP_TOKEN_MINUS,
    CppLexer::CPP_TOKEN_FLOATING_CONSTANT
  };
  static CppLexer::Cpp_Token_Type r32_sequence_c[] = {
    CppLexer::CPP_TOKEN_PLUS,
    CppLexer::CPP_TOKEN_FLOATING_CONSTANT
  };

  static CppLexer::Cpp_Token_Type b32_sequence[] = {
    CppLexer::CPP_TOKEN_BOOLEAN_CONSTANT
  };

  static CppLexer::Cpp_Token_Type char_sequence_a[] = {
    CppLexer::CPP_TOKEN_CHARACTER_CONSTANT
  };
  static CppLexer::Cpp_Token_Type char_sequence_b[] = {
    CppLexer::CPP_TOKEN_INTEGER_CONSTANT
  };

  if (string_eq(type_name, STRING("uint32_t")) &&
      token_sequence_matches(tokens, token_range, ARRAY_SIZE(u32_sequence), u32_sequence))
  {
    success &= parse_u32(&token_text, (u32*)result);
  }
  else
  if (string_eq(type_name, STRING("int32_t")) &&
      (token_sequence_matches(tokens, token_range, ARRAY_SIZE(s32_sequence_a), s32_sequence_a) ||
       token_sequence_matches(tokens, token_range, ARRAY_SIZE(s32_sequence_b), s32_sequence_b) ||
       token_sequence_matches(tokens, token_range, ARRAY_SIZE(s32_sequence_c), s32_sequence_c)))
  {
    success &= parse_s32(&token_text, (s32*)result);
  }
  else
  if (string_eq(type_name, STRING("float")) &&
      (token_sequence_matches(tokens, token_range, ARRAY_SIZE(r32_sequence_a), r32_sequence_a) ||
       token_sequence_matches(tokens, token_range, ARRAY_SIZE(r32_sequence_b), r32_sequence_b) ||
       token_sequence_matches(tokens, token_range, ARRAY_SIZE(r32_sequence_c), r32_sequence_c)))
  {
    success &= parse_r32(&token_text, (r32*)result);
  }
  else
  if (string_eq(type_name, STRING("bool")) &&
      token_sequence_matches(tokens, token_range, ARRAY_SIZE(b32_sequence), b32_sequence))
  {
    printf("Parsing bool:  Unimplemented\n");
    success &= false;
    // success &= parse_b32(&token_text, (b32*)result);
  }
  else
  if (string_eq(type_name, STRING("char")) &&
      (token_sequence_matches(tokens, token_range, ARRAY_SIZE(char_sequence_a), char_sequence_a) ||
       token_sequence_matches(tokens, token_range, ARRAY_SIZE(char_sequence_b), char_sequence_b)))
  {
    success &= parse_char(&token_text, (char*)result);
  }
  else
  {
    success = false;
  }

  if (!success)
  {
    printf("Parsing error for type %.*s.\n", STR_PRINT(type_name));
  }

  return success;
}


b32
parse_value_tokens(String text, TokenArray tokens, TokenArrayRange token_range, String type_name, void *result, StructAnnotations& struct_annotations)
{
  b32 success = true;

  // printf("Tokens for %.*s value: ", STR_PRINT(type_name));  print_token_range_string(text, tokens, token_range);  printf("\n");

  StructAnnotation *struct_annotation = get_struct_annotation(struct_annotations, type_name);

  String deserializable_type_name = type_name;

  if (struct_annotation != NULL &&
      struct_annotation->type_alias)
  {
    deserializable_type_name = struct_annotation->aliased_type;
    struct_annotation = NULL;
  }

  if (struct_annotation == NULL)
  {
    success &= parse_atomic_type_tokens(text, tokens, token_range, deserializable_type_name, result);
  }
  else
  {
    // Parse "struct type_name {...}"

    TokenArrayRange struct_contents_range = get_struct_contents(text, tokens, token_range, struct_annotation->type_name);
    if (struct_contents_range.start_token == -1)
    {
      success = false;
    }
    else
    {
      for (u32 member_i = 0;
           member_i < struct_annotation->members.n_elements;
           ++member_i)
      {
        StructAnnotationMember& member = struct_annotation->members[member_i];

        TokenArrayRange member_value_tokens_range = find_value_in_tokens(text, tokens, struct_contents_range, member.type_name, member.label);
        if (member_value_tokens_range.start_token == -1)
        {
          success = false;
        }
        else
        {
          void *member_data = (u8*)result + member.offset;
          success &= parse_value_tokens(text, tokens, member_value_tokens_range, member.type_name, member_data, struct_annotations);
        }

        if (!success)
        {
          printf("Failed to deserialize %.*s %.*s.\n", STR_PRINT(member.type_name), STR_PRINT(member.label));
          break;
        }
      }
    }
  }

  return success;
}


b32
deserialize_value(String text, String type_name, String label, void *result, StructAnnotations& struct_annotations)
{
  b32 success = true;

  // TODO:  Maybe don't regenerate the tokens every time the function is called, just once per file.

  CppLexer::Cpp_Token_Array cpp_tokens = CppLexer::cpp_make_token_array(100);
  CppLexer::cpp_lex_file((char *)text.start, STR_LENGTH(text), &cpp_tokens);

  TokenArray tokens = {
    .tokens = cpp_tokens.tokens,
    .count = cpp_tokens.count
  };

  TokenArrayRange value_tokens_range = find_value_in_tokens(text, tokens, {0, tokens.count}, type_name, label);
  if (value_tokens_range.start_token == -1)
  {
    success = false;
  }
  else
  {
    success &= parse_value_tokens(text, tokens, value_tokens_range, type_name, result, struct_annotations);
  }

  return success;
}