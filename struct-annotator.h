#ifndef STRUCT_ANNOTATOR_H_DEF
#define STRUCT_ANNOTATOR_H_DEF

#include "string.h"
#include "array.h"

#include "serializable-types.h"


struct StructAnnotationMember
{
  u32 offset;
  String name;
  SerializableType type;
};


struct StructAnnotation
{
  String name;
  u32 size;
  Array::Array<StructAnnotationMember> members;
};


#define TOKEN_TO_STRING(token) #token


// Used to create the struct {...} definition for each member of the struct
#define ANNOTATED_STRUCT_MEMBER_DEF(member_type, member_name, struct_name) member_type member_name;

// Used to create the annotation for each member of the struct in the annotation function
#define ANNOTATED_STRUCT_MEMBER_ANNOTATION(member_type, member_name, struct_name)\
{                                                                                \
  StructAnnotationMember member = {};                                            \
                                                                                 \
  member.offset = offsetof(struct_name, member_name);                            \
  STR_SET(member.name, TOKEN_TO_STRING(member_name));                            \
  member.type = SerializableType::member_type;                                   \
                                                                                 \
  Array::add(result.members, member);                                            \
}

// Defines the struct {...} definition and the struct get_annotation function
#define ANNOTATED_STRUCT(struct_name, struct_members)                            \
struct struct_name                                                               \
{                                                                                \
  struct_members(ANNOTATED_STRUCT_MEMBER_DEF, struct_name)                       \
};                                                                               \
                                                                                 \
static StructAnnotation                                                          \
get_annotated_##struct_name()                                                    \
{                                                                                \
  StructAnnotation result = {};                                                  \
  STR_SET(result.name, TOKEN_TO_STRING(struct_name));                            \
                                                                                 \
  result.size = sizeof(struct_name);                                             \
                                                                                 \
  struct_members(ANNOTATED_STRUCT_MEMBER_ANNOTATION, struct_name)                \
                                                                                 \
  return result;                                                                 \
}


void
print_struct_annotation(StructAnnotation& struct_annotation);


void
print_annotated_struct(StructAnnotation& struct_annotation, void *data);


#endif