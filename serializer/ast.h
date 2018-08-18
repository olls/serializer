#ifndef AST_H_DEF
#define AST_H_DEF

#include "../libs/types.h"
#include "../libs/string.h"
#include "../libs/array.h"
#include "tokens.h"

// Generate an AST for the simple C-style text format.
//
// Nodes can be of AST_NodeType: Declaration, Literal, or Struct:
// - Declaration:
//   - Stores the TokenRange for a values type and identifier.
//   - Stores the node index of the declaration's value - which must be a Literal or Struct AST_NodeType.
//
// - Literal:
//   - An "atomic" data type.  Just stores the TokenRange containing the value.
//
// - Struct:
//   - A recursive type.
//   - Stores the type_name specified in the `struct type_name {...}` syntax.  Note that this is not checked for
//       consistency with the type name specified in the nodes parent declaration.
//   - Stores an Array of node indices for all it's members.
//
// generate_ast()  recursively parses declarations from a TokenArray returning an AST object, which contains an Array of
//   AST_Node's and the index of the root struct AST_Node.
//


enum struct AST_NodeType
{
  Declaration,
  Literal,
  Struct
};


struct AST_Node
{
  AST_NodeType type;
  TokenRange token_range;

  union
  {
    struct
    {
      CppLexer::Cpp_Token type;
      CppLexer::Cpp_Token identifier;
      u32 value_node;
    } declaration_type;

    struct
    {
      CppLexer::Cpp_Token type;
      Array::Array<u32> members;
    } struct_type;
  };
};


struct AST
{
  u32 root_node_index;
  Array::Array<AST_Node> nodes;
};


b32
ast_find_declarations(String text, CppLexer::Cpp_Token_Array tokens, AST *ast, u32 struct_node);


b32
generate_ast(String text, CppLexer::Cpp_Token_Array cpp_tokens, AST *result);


void
print_all_ast_nodes(AST& ast, String text, CppLexer::Cpp_Token_Array tokens);


void
print_ast(AST& ast, String text, CppLexer::Cpp_Token_Array tokens, u32 indent = 0, u32 node_index = 0);


#endif