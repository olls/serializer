#include "deserializer.h"

#include "value-parser.h"
#include "serializable-types.h"
#include "tokens.h"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))


// Searches the token array for a matching 'type label =' at the current level scope of the passed in tokens, returns
//   the token range of its value.
//
TokenRange
find_value_in_tokens(String text, CppLexer::Cpp_Token_Array tokens, TokenRange token_range, const char *type_name, const char *label, u32 array_size)
{
  TokenRange result = {-1};

  assert(token_range.start_token >= 0);
  assert(token_range.end_token <= tokens.count);

  // Search tokens for a matching "type_name label = " or (if array_size > 1) "type_name label[array_size] = ", skipping
  //   any braced sections to stay at the top level scope.  Once found, return all tokens from label to the next ";"
  //   which is at __the same scope level as the label__.

  // Need two sequences because the type can be either KEY_TYPE, or IDENTIFIER depending on whether it is an built-in or
  //   user type...
  CppLexer::Cpp_Token_Type type_ident_seq_a[] = {
    CppLexer::CPP_TOKEN_KEY_TYPE,
    CppLexer::CPP_TOKEN_IDENTIFIER
  };
  CppLexer::Cpp_Token_Type type_ident_seq_b[] = {
    CppLexer::CPP_TOKEN_IDENTIFIER,
    CppLexer::CPP_TOKEN_IDENTIFIER
  };

  u32 sequence_length = ARRAY_SIZE(type_ident_seq_a);
  assert(sequence_length == ARRAY_SIZE(type_ident_seq_b));

  // Search for "type_name label = "
  for (s32 token_index = token_range.start_token;
       token_index < token_range.end_token;
       ++token_index)
  {
    token_index = skip_scoped_sections(tokens, token_index, token_range.end_token);
    if (token_index == -1)
    {
      break;
    }

    if ((token_sequence_matches(tokens, {token_index, token_range.end_token}, sequence_length, type_ident_seq_a) ||
         token_sequence_matches(tokens, {token_index, token_range.end_token}, sequence_length, type_ident_seq_b)) &&
        string_eq(token_string(text, tokens.tokens[token_index + 0]), type_name) &&
        string_eq(token_string(text, tokens.tokens[token_index + 1]), label))
    {
      token_index += sequence_length;

      CppLexer::Cpp_Token_Type array_seq[] = {
        CppLexer::CPP_TOKEN_BRACKET_OPEN,
        CppLexer::CPP_TOKEN_INTEGER_CONSTANT,
        CppLexer::CPP_TOKEN_BRACKET_CLOSE
      };

      b32 array_match = true;
      if (array_size > 1)
      {
        if (token_sequence_matches(tokens, {token_index, token_range.end_token}, ARRAY_SIZE(array_seq), array_seq))
        {
          u32 found_array_size;
          String found_array_size_string = token_string(text, tokens.tokens[token_index + 1]);

          if (parse_u32(&found_array_size_string, &found_array_size) &&
              found_array_size == array_size)
          {
            token_index += ARRAY_SIZE(array_seq);
          }
        }
        else
        {
          array_match = false;
        }
      }

      if (array_match &&
          token_index < token_range.end_token &&
          tokens.tokens[token_index].type == CppLexer::CPP_TOKEN_EQ)
      {
        token_index += 1;

        // Found start of the matching value
        result.start_token = token_index;
        break;
      }
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
      token_index = skip_scoped_sections(tokens, token_index, token_range.end_token);
      if (token_index == -1)
      {
        result.start_token = -1;
        break;
      }

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


TokenRange
get_struct_contents(String text, CppLexer::Cpp_Token_Array tokens, TokenRange token_range, const char *type_name)
{
  TokenRange result = {-1};

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
    result.end_token = find_end_scope_token(tokens, {result.start_token-1, token_range.end_token},
                                            CppLexer::CPP_TOKEN_BRACE_OPEN,
                                            CppLexer::CPP_TOKEN_BRACE_CLOSE);
    if (result.end_token == -1)
    {
      result.start_token = -1;
      printf("No matching {} found.\n");
    }
  }

  return result;
}


TokenRange
get_array_contents(CppLexer::Cpp_Token_Array tokens, TokenRange token_range)
{
  TokenRange result = {-1};

  if (token_range.start_token < token_range.end_token &&
      tokens.tokens[token_range.start_token].type == CppLexer::CPP_TOKEN_BRACKET_OPEN)
  {
    result.start_token = token_range.start_token + 1;
    result.end_token = find_end_scope_token(tokens, {token_range.start_token, token_range.end_token},
                                            CppLexer::CPP_TOKEN_BRACKET_OPEN,
                                            CppLexer::CPP_TOKEN_BRACKET_CLOSE);
    if (result.end_token == -1)
    {
      result.start_token = -1;
      printf("No matching [] found.\n");
    }
  }

  return result;
}


b32
parse_atomic_type_tokens(String text, CppLexer::Cpp_Token_Array tokens, TokenRange token_range, const char *type_name, void *result)
{
  b32 success = true;

  // TODO:  This could be rewritten to be more extensible and use the tokens for parsing the values more nicely

  // TODO:  Error if there is junk left over in token_range after parsing value, this would catch errors where too many
  //          tokens were parsed as a value

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

  if (strcmp(type_name, "uint32_t") == 0 &&
      token_sequence_matches(tokens, token_range, ARRAY_SIZE(u32_sequence), u32_sequence))
  {
    success &= parse_u32(&token_text, (u32*)result);
  }
  else
  if (strcmp(type_name, "int32_t") == 0 &&
      (token_sequence_matches(tokens, token_range, ARRAY_SIZE(s32_sequence_a), s32_sequence_a) ||
       token_sequence_matches(tokens, token_range, ARRAY_SIZE(s32_sequence_b), s32_sequence_b) ||
       token_sequence_matches(tokens, token_range, ARRAY_SIZE(s32_sequence_c), s32_sequence_c)))
  {
    success &= parse_s32(&token_text, (s32*)result);
  }
  else
  if (strcmp(type_name, "float") == 0 &&
      (token_sequence_matches(tokens, token_range, ARRAY_SIZE(r32_sequence_a), r32_sequence_a) ||
       token_sequence_matches(tokens, token_range, ARRAY_SIZE(r32_sequence_b), r32_sequence_b) ||
       token_sequence_matches(tokens, token_range, ARRAY_SIZE(r32_sequence_c), r32_sequence_c)))
  {
    success &= parse_r32(&token_text, (r32*)result);
  }
  else
  if (strcmp(type_name, "bool") == 0 &&
      token_sequence_matches(tokens, token_range, ARRAY_SIZE(b32_sequence), b32_sequence))
  {
    printf("Parsing bool:  Unimplemented\n");
    success &= false;
    // success &= parse_b32(&token_text, (b32*)result);
  }
  else
  if (strcmp(type_name, "char") == 0 &&
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
    printf("Parsing error for type %s.\n", type_name);
  }

  return success;
}


b32
parse_value_tokens(String text, CppLexer::Cpp_Token_Array tokens, TokenRange token_range, const char *type_name, u32 array_size, void *result, StructAnnotations& struct_annotations)
{
  b32 success = true;

  // printf("Tokens for %s[%u] value: ", type_name, array_size);  print_token_range_string(text, tokens, token_range);  printf("\n");

  StructAnnotation *struct_annotation = get_struct_annotation(struct_annotations, type_name);
  const char *aliased_type_name = type_name;
  while (struct_annotation != NULL &&
         struct_annotation->type_alias)
  {
    aliased_type_name = struct_annotation->aliased_type;
    struct_annotation = get_struct_annotation(struct_annotations, aliased_type_name);
  }

  u32 element_size;
  SerializableType no_annotation_type;
  if (struct_annotation == NULL)
  {
    if (serializable_type(aliased_type_name, &no_annotation_type))
    {
      element_size = serializable_type_size(no_annotation_type);
    }
    else
    {
      element_size = 0;
    }
  }
  else
  {
    element_size = struct_annotation->size;
  }

  TokenRange remaining_value_range = token_range;
  if (array_size > 1)
  {
    // Get inner [...]
    remaining_value_range = get_array_contents(tokens, token_range);
  }

  if (remaining_value_range.start_token == -1)
  {
    success = false;
  }
  else
  {
    for (u32 array_element_index = 0;
         array_element_index < array_size;
         ++array_element_index)
    {
      void *array_element_data = (u8*)result + (array_element_index * element_size);

      // Get token range for next element in array
      TokenRange single_value_contents_range = remaining_value_range;
      if (array_size > 1 &&
          array_element_index < array_size - 1)
      {
        single_value_contents_range = find_next_token_at_matching_scope(tokens, remaining_value_range, CppLexer::CPP_TOKEN_COMMA);
        if (single_value_contents_range.start_token == -1)
        {
          success = false;
          break;
        }

        remaining_value_range.start_token = single_value_contents_range.end_token + 1;
      }

      if (struct_annotation == NULL)
      {
        success &= parse_atomic_type_tokens(text, tokens, single_value_contents_range, aliased_type_name, array_element_data);
      }
      else
      {
        // Parse "struct type_name {...}"

        TokenRange struct_contents_range = get_struct_contents(text, tokens, single_value_contents_range, type_name);
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

            TokenRange member_value_tokens_range = find_value_in_tokens(text, tokens, struct_contents_range, member.type_name, member.label, member.array_size);
            if (member_value_tokens_range.start_token == -1)
            {
              success = false;
            }
            else
            {
              void *member_data = (u8*)array_element_data + member.offset;
              success &= parse_value_tokens(text, tokens, member_value_tokens_range, member.type_name, member.array_size, member_data, struct_annotations);
            }

            if (!success)
            {
              printf("Failed to deserialize %s %s.\n", member.type_name, member.label);
              break;
            }
          }
        }
      }
    }
  }

  return success;
}


b32
deserialize_value(String text, const char *type_name, const char *label, void *result, StructAnnotations& struct_annotations)
{
  b32 success = true;

  // TODO:  Maybe don't regenerate the tokens every time the function is called, just once per file.

  CppLexer::Cpp_Token_Array tokens = CppLexer::cpp_make_token_array(100);
  CppLexer::cpp_lex_file((char *)text.start, STR_LENGTH(text), &tokens);

  TokenRange value_tokens_range = find_value_in_tokens(text, tokens, {0, tokens.count}, type_name, label, 1);
  if (value_tokens_range.start_token == -1)
  {
    success = false;
  }
  else
  {
    success &= parse_value_tokens(text, tokens, value_tokens_range, type_name, 1, result, struct_annotations);
  }

  return success;
}