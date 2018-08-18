# Serializer

A simple C++ serializer for saving and loading struct data to and from text.


## Use

### Serialization

`serialize_data()` is used to convert a data type into text.

    void serialize_data(const char *type_name, const char *label, void *data, FILE *output = stdout);

- `type_name`:  The C-string containing the type name of the data you are outputting.
- `label`:      A C-string containing a label for the object you are outputting, this is the same label you use to
                  retrieve the data from the text when deserializing.
- `data`:       Pointer to the data you are outputting.  This is assumed have size equal to `sizeof(type_name)`.
- `output`:     The FILE stream to output the serialized data to.  This is where you can put your file stream to output
                  to a file.

#### Example

    r32 pizza_time = 12.5;
    serialize_data("s32", "pizza_time", &pizza_time);

Will print `r32 pizza_time = 12.500000;` to stdout:


### Deserialization

`deserialize_value()` is used to read serialized data back in from text data.

    b32 deserialize_value(String text, const char *type_name, const char *label, void *result);

- `text`:       The text containing the serialized values.
- `type_name`:  A C-string containing the type name of the value you wish to read in.  This should be the same type used
                  for the `serialize_data()` `type_name` parameter.
- `label`:      A C-string containing the label of the value you wish to read in.  This should be the same type used for
                  the `serialize_data()` `label` parameter.
- `result`:     A pointer to a `type_name` object, or memory with size equal to `sizeof(type_name)`.

#### Example

    String file_text = STRING("r32 pizza_time = 12.500000;");

    r32 pizza_time_deserialized;
    deserialize_value(file_text, "r32", "pizza_time", &pizza_time_deserialized);

    printf("pizza_time: %f\n", pizza_time_deserialized);

`deserialize_value()` will search the string for the `pizza_time` label, and output `pizza_time: 12.500000` to stdout.


### Annotated Typedefs

Typedefs can be annotated so that the typedef can be used in serialization and deserialization, preserving their
typedef-ed type.  Annotated typedefs can be created using the following macro, this defines the typedef for you.

    ANNOTATED_TYPEDEF(float, MyFloat);

Annotated typedefs can be used for any type or struct which is also annotated.


### Annotated Structs

The serialize and deserialization of data types can be used on struct types as well.  To use a struct it must be
annotated using the preprocessor, this will store details about the member types, names, and offsets in the struct in a
global hashmap for the deserialization to make use of.

To create annotated structs, you must define your structs using the preprocessor macros as in the following example:

    // Create a macro which will generate the struct members when called:
    #define Entity_DEF(M, n) \
      M(vec2, position, n) \
      M(vec2, velocity, n) \
      M(u32, id, n) \
      M(char, letter, n)

    // Pass your macro to ANNOTATED_STRUCT to generate both the struct {}; definition, and the annotations.
    ANNOTATED_STRUCT(Entity, Entity_DEF);

The first argument passed to `ANNOTATED_STRUCT()` is the struct name which will be used to define it.  The example above
would output a struct definition which looks like this:

    struct Entity {
      vec2 position;
      vec2 velocity;
      u32 id;
      char letter;
    };

Note that recursive struct (de)serialization requires all struct-type members used in a struct definition to be defined
using the annotation macros as well.

`example/main.cpp` and `example/test-file.h` contain a setup demonstrating all the features of the library, and how to
use it to save and load data from a file.


## Unimplemented features

Currently the following items are not implemented, hopefully they will be in the future:
- Strings
- Enums
- Struct unions