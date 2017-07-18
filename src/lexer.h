#if !defined(_LEXER_H)

#include "types.h"
#include "utilities.h"
#include "platform.h"

enum Modifier {
    Modifier_unknown  = 0x00,

    Modifier_unsigned = 0x01,
    Modifier_const    = 0x02,
    Modifier_volatile = 0x04,
    Modifier_mutable  = 0x08,
    Modifier_signed   = 0x10,
};

enum Struct_Type {
    StructType_unknown,
    StructType_struct,
    StructType_class,
    StructType_union,

    StructType_count
};

struct Struct_Data {
    String template_header;
    String name;
    Int member_count;
    Variable *members;

    Int inherited_count;
    String *inherited;

    Struct_Type struct_type;
};

struct Structs {
    Struct_Data *e;
    Int cnt;
};

struct Enum_Value {
    String name;
    Int value;
};

struct Enum_Data {
    String name;
    String type;
    Bool is_struct;

    Enum_Value *values;
    Int no_of_values;
};

struct Enums {
    Enum_Data *e;
    Int cnt;
};

struct Function_Data {
    String linkage;
    String return_type;
    Int return_type_ptr;
    String name;

    Variable *params;
    Int param_cnt;
};

struct Typedef_Data {
    String original;
    String fresh;
};

struct Functions {
    Function_Data *e;
    Int cnt;
};

struct Typedefs {
    Typedef_Data *e;
    Int cnt;
};

struct Parse_Result {
    Structs structs;
    Enums enums;
    Functions funcs;
    Typedefs typedefs;

    Int struct_max;
    Int enum_max;
    Int func_max;
    Int typedef_max;
};

Parse_Result parse_streams(Int cnt, Char **fnames);

//
// The following are only exported for test.c
//
struct Tokenizer {
    Char *at;
};

struct Parse_Struct_Result {
    Struct_Data sd;
    Bool success;
};
Parse_Struct_Result parse_struct(Tokenizer *tokenizer, Struct_Type struct_type);

#define eat_token(tokenizer) eat_tokens(tokenizer, 1)
Void eat_tokens(Tokenizer *tokenizer, Int num_tokens_to_eat);

File preprocess_macros(File file);

#define _LEXER_H
#endif