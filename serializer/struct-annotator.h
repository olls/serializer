#ifndef STRUCT_ANNOTATOR_H_DEF
#define STRUCT_ANNOTATOR_H_DEF

#include "../libs/array.h"
#include "../libs/hashmap.h"

#include "serializable-types.h"


struct StructAnnotationMember
{
  u32 offset;
  u32 array_size;
  const char *type_name;
  const char *label;
};


struct StructAnnotation
{
  const char *type_name;
  u32 size;

  b32 type_alias;

  union
  {
    Array::Array<StructAnnotationMember> members;
    const char *aliased_type;
  };
};


static struct StructAnnotations
{
  Hashmap::Hashmap<char, StructAnnotation> map;
} global_struct_annotations = {};


StructAnnotation *
get_struct_annotation(StructAnnotations& struct_annotations, const char *annotation_name);


#define TOKEN_TO_STRING(token) #token


// Used to create the struct {...} definition for each member of the struct
#define ANNOTATED_STRUCT_MEMBER_DEF(member_type, member_label, struct_name) member_type member_label;
#define ANNOTATED_STRUCT_MEMBER_ARRAY_DEF(member_type, member_label, array_size, struct_name) member_type member_label[array_size];

// Used to create the annotation for each member of the struct in the annotation function
#define ANNOTATED_STRUCT_MEMBER_ANNOTATION(member_type, member_label, struct_name) \
{                                                                                \
  StructAnnotationMember member = {};                                            \
                                                                                 \
  member.offset = offsetof(struct_name, member_label);                           \
  member.array_size = 1;                                                         \
  member.label = TOKEN_TO_STRING(member_label);                                  \
  member.type_name = TOKEN_TO_STRING(member_type);                               \
                                                                                 \
  Array::add(annotation.members, member);                                        \
}

#define ANNOTATED_STRUCT_MEMBER_ARRAY_ANNOTATION(member_type, member_label, array_size_, struct_name) \
{                                                                                \
  StructAnnotationMember member = {};                                            \
                                                                                 \
  member.offset = offsetof(struct_name, member_label);                           \
  member.array_size = array_size_;                                               \
  member.label = TOKEN_TO_STRING(member_label);                                  \
  member.type_name = TOKEN_TO_STRING(member_type);                               \
                                                                                 \
  Array::add(annotation.members, member);                                        \
}

// Defines the struct {...} definition and the struct add_annotation function
#define ANNOTATED_STRUCT(struct_name, struct_members)                            \
struct struct_name                                                               \
{                                                                                \
  struct_members(ANNOTATED_STRUCT_MEMBER_DEF, ANNOTATED_STRUCT_MEMBER_ARRAY_DEF, struct_name) \
};                                                                               \
                                                                                 \
static const char *                                                              \
add_annotated__##struct_name(StructAnnotations *struct_annotations)              \
{                                                                                \
  const char struct_name_str[] = TOKEN_TO_STRING(struct_name);                   \
                                                                                 \
  StructAnnotation &annotation = Hashmap::set(struct_annotations->map,           \
    struct_name_str, strlen(struct_name_str));                                   \
                                                                                 \
  annotation.type_name = TOKEN_TO_STRING(struct_name);                           \
                                                                                 \
  annotation.type_alias = false;                                                 \
  annotation.members = {};                                                       \
  annotation.size = sizeof(struct_name);                                         \
                                                                                 \
  struct_members(ANNOTATED_STRUCT_MEMBER_ANNOTATION, ANNOTATED_STRUCT_MEMBER_ARRAY_ANNOTATION, struct_name) \
                                                                                 \
  return annotation.type_name;                                                   \
}                                                                                \
                                                                                 \
static const char *struct_name##__annotation_type_name = add_annotated__##struct_name(&global_struct_annotations)


#define ANNOTATED_TYPEDEF(aliased_type_name, type_name_alias)                    \
typedef aliased_type_name type_name_alias;                                       \
                                                                                 \
static const char *                                                              \
add_annotated_typedef__##type_name_alias(StructAnnotations *struct_annotations)  \
{                                                                                \
  const char type_name_alias_cstr[] = TOKEN_TO_STRING(type_name_alias);          \
                                                                                 \
  StructAnnotation& annotation = Hashmap::set(struct_annotations->map,           \
    type_name_alias_cstr, strlen(type_name_alias_cstr));                         \
                                                                                 \
  annotation.type_name = TOKEN_TO_STRING(type_name_alias);                       \
                                                                                 \
  annotation.type_alias = true;                                                  \
  annotation.size = sizeof(type_name_alias);                                     \
  annotation.aliased_type = TOKEN_TO_STRING(aliased_type_name);                  \
                                                                                 \
  return annotation.type_name;                                                   \
}                                                                                \
                                                                                 \
static const char *type_name_alias##__annotation_type_name = add_annotated_typedef__##type_name_alias(&global_struct_annotations)


void
print_struct_annotation(const char *struct_annotation_name, FILE *output = stdout, StructAnnotations& struct_annotations = global_struct_annotations, u32 indent = 0);


ANNOTATED_TYPEDEF(uint64_t, u64);
ANNOTATED_TYPEDEF(uint32_t, u32);
ANNOTATED_TYPEDEF(uint16_t, u16);
ANNOTATED_TYPEDEF(uint8_t, u8);

ANNOTATED_TYPEDEF(int64_t, s64);
ANNOTATED_TYPEDEF(int32_t, s32);
ANNOTATED_TYPEDEF(int16_t, s16);
ANNOTATED_TYPEDEF(int8_t, s8);

ANNOTATED_TYPEDEF(float, r32);
ANNOTATED_TYPEDEF(double, r64);

ANNOTATED_TYPEDEF(u32, b32);


#endif