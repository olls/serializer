#include "ast.h"
#include "serializer.h"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))


b32
ast_parse_value(String text, CppLexer::Cpp_Token_Array tokens, TokenRange token_range, AST *ast, u32 value_node)
{
  b32 success = true;
  TokenRange struct_contents = {-1};

  // Test if value has struct syntax, otherwise just return AST_NodeType::Literal.

  CppLexer::Cpp_Token_Type struct_decl_seq[] = {
    CppLexer::CPP_TOKEN_KEY_TYPE_DECLARATION,
    CppLexer::CPP_TOKEN_IDENTIFIER,
    CppLexer::CPP_TOKEN_BRACE_OPEN
  };

  if (token_sequence_matches(tokens, token_range, ARRAY_SIZE(struct_decl_seq), struct_decl_seq))
  {
    // Found: 'struct type_name {'
    struct_contents.start_token = token_range.start_token + ARRAY_SIZE(struct_decl_seq);
    struct_contents.end_token = find_end_scope_token(tokens, {struct_contents.start_token-1, token_range.end_token},
                                            CppLexer::CPP_TOKEN_BRACE_OPEN,
                                            CppLexer::CPP_TOKEN_BRACE_CLOSE);
    if (struct_contents.end_token == -1)
    {
      success &= false;
      struct_contents.start_token = -1;
      printf("No matching {} found.\n");
    }
  }

  if (struct_contents.start_token != -1)
  {
    printf("New Struct node\n");

    ast->nodes[value_node] = {
      .type = AST_NodeType::Struct,
      .token_range = struct_contents,

      .struct_type = {
        .type = tokens.tokens[token_range.start_token + 1],
        .members = {}
      }
    };

    Array::clear(ast->nodes[value_node].struct_type.members);

    ast_find_declarations(text, tokens, ast, value_node);
  }
  else
  {
    printf("New Literal node\n");
    ast->nodes[value_node] = {
      .type = AST_NodeType::Literal,
      .token_range = token_range
    };
  }

  return success;
}


b32
ast_find_declarations(String text, CppLexer::Cpp_Token_Array tokens, AST *ast, u32 struct_node)
{
  b32 success = true;

  CppLexer::Cpp_Token_Type type_identifier_seq_a[] = {
    CppLexer::CPP_TOKEN_IDENTIFIER,
    CppLexer::CPP_TOKEN_IDENTIFIER,
    CppLexer::CPP_TOKEN_EQ
  };
  CppLexer::Cpp_Token_Type type_identifier_seq_b[] = {
    CppLexer::CPP_TOKEN_KEY_TYPE_DECLARATION,
    CppLexer::CPP_TOKEN_IDENTIFIER,
    CppLexer::CPP_TOKEN_EQ
  };
  const s32 seq_length = ARRAY_SIZE(type_identifier_seq_a);
  static_assert(seq_length == ARRAY_SIZE(type_identifier_seq_b), "Sequences should have matching lengths");

  for (s32 token_index = ast->nodes[struct_node].token_range.start_token;
       token_index < ast->nodes[struct_node].token_range.end_token;
       ++token_index)
  {
    printf("Testing tokens:  ");
    print_token_range_string(text, tokens, {token_index, token_index + 3});

    if (token_sequence_matches(tokens, {token_index, ast->nodes[struct_node].token_range.end_token}, seq_length, type_identifier_seq_a) ||
        token_sequence_matches(tokens, {token_index, ast->nodes[struct_node].token_range.end_token}, seq_length, type_identifier_seq_b))
    {
      s32 declaration_start_token = token_index;
      token_index += seq_length;

      if (token_index < ast->nodes[struct_node].token_range.end_token)
      {
        printf("Found type id:  ");
        print_token_range_string(text, tokens, {declaration_start_token, declaration_start_token+seq_length});

        // Find end of the declaration's value

        TokenRange value_range = find_next_token_at_matching_scope(tokens, {token_index, ast->nodes[struct_node].token_range.end_token}, CppLexer::CPP_TOKEN_SEMICOLON);
        if (value_range.start_token == -1)
        {
          printf("Couldn't find end of declaration.\n");
          success = false;
          break;
        }
        else
        {
          TokenRange declaration_range = {declaration_start_token, value_range.end_token};

          u32 declaration_node = Array::new_position(ast->nodes);
          Array::new_element(ast->nodes[struct_node].struct_type.members) = declaration_node;

          printf("New declaration node %u\n", declaration_node);

          ast->nodes[declaration_node] = {
            .type = AST_NodeType::Declaration,
            .token_range = declaration_range,

            .declaration_type = {
              .type = tokens.tokens[declaration_range.start_token],
              .identifier = tokens.tokens[declaration_range.start_token + 1]
            }
          };

          u32 declaration_value_node = Array::new_position(ast->nodes);
          ast->nodes[declaration_node].declaration_type.value_node = declaration_value_node;

          success &= ast_parse_value(text, tokens, value_range, ast, declaration_value_node);

          token_index = declaration_range.end_token;
        }
      }
    }
  }

  return success;
}


b32
generate_ast(String text, CppLexer::Cpp_Token_Array tokens, AST *ast)
{
  b32 success = true;

  Array::clear(ast->nodes);
  ast->root_node_index = Array::new_position(ast->nodes);

  ast->nodes[ast->root_node_index].type = AST_NodeType::Struct;
  ast->nodes[ast->root_node_index].token_range = {0, tokens.count};
  Array::clear(ast->nodes[ast->root_node_index].struct_type.members);

  success = ast_find_declarations(text, tokens, ast, ast->root_node_index);

  return success;
}


void
print_ast_node(AST_Node& node, String text, CppLexer::Cpp_Token_Array tokens, u32 indent = 0)
{
  switch (node.type)
  {
    case (AST_NodeType::Declaration):
      printf("%*sDeclaration\n", indent*2, "");
      printf("%*stype_name:  \"%.*s\"\n", indent*2, "", STR_PRINT(token_string(text, node.declaration_type.type)));
      printf("%*sidentifier:  \"%.*s\"\n", indent*2, "", STR_PRINT(token_string(text, node.declaration_type.identifier)));
      printf("%*svalue_node:  %u\n", indent*2, "", node.declaration_type.value_node);
      break;
    case (AST_NodeType::Literal):
      printf("%*sLiteral\n", indent*2, "");
      printf("%*svalue: ", indent*2, "");
      print_token_range_string(text, tokens, node.token_range);
      break;
    case (AST_NodeType::Struct):
      printf("%*sStruct\n", indent*2, "");
      printf("%*schildren (%u):  ", indent*2, "", node.struct_type.members.n_elements);
      for (u32 node_children_index = 0;
           node_children_index < node.struct_type.members.n_elements;
           ++node_children_index)
      {
        printf("%u, ", node.struct_type.members[node_children_index]);
      }
      printf("\n");
      break;
  }

  // printf("  tokens:  ");
  // print_token_range(tokens, node.token_range);

  printf("%*stext:  ", indent*2, "");
  print_token_range_string(text, tokens, node.token_range);
}


void
print_all_ast_nodes(AST& ast, String text, CppLexer::Cpp_Token_Array tokens)
{
  for (u32 node_index = 0;
       node_index < ast.nodes.n_elements;
       ++node_index)
  {
    printf("node %u: \n", node_index);
    print_ast_node(ast.nodes[node_index], text, tokens, 1);
  }
}


void
print_ast(AST& ast, String text, CppLexer::Cpp_Token_Array tokens, u32 indent, u32 node_index)
{
  if (indent == 0)
  {
    node_index = ast.root_node_index;
  }

  printf("%*sNode %u\n", indent*2, "", node_index);
  printf("%*s{\n", indent*2, "");

  AST_Node& node = ast.nodes[node_index];
  print_ast_node(node, text, tokens, indent+1);

  if (node.type == AST_NodeType::Declaration)
  {
    assert(!(node.declaration_type.value_node == 0));
    print_ast(ast, text, tokens, indent+1, node.declaration_type.value_node);
  }
  else if (node.type == AST_NodeType::Struct)
  {
    for (u32 node_member_index = 0;
         node_member_index < node.struct_type.members.n_elements;
         ++node_member_index)
    {
      print_ast(ast, text, tokens, indent+1, node.struct_type.members[node_member_index]);
    }
  }
  printf("%*s}\n", indent*2, "");
}