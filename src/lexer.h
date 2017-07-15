#if !defined(_LEXER_H)

#include "types.h"
#include "utilities.h"
#include "platform.h"

typedef enum {
    Modifier_unknown = 0x00,

    Modifier_unsigned = 0x01,
    Modifier_const    = 0x02,
    Modifier_volatile = 0x04,
    Modifier_mutable  = 0x08,
    Modifier_signed   = 0x10,
} Modifier;

typedef enum {
    StructType_unknown,
    StructType_struct,
    StructType_class,
    StructType_union,

    StructType_count
} Struct_Type;

typedef struct {
    String template_header;
    String name;
    Int member_count;
    Variable *members;

    Int inherited_count;
    String *inherited;

    Struct_Type struct_type;
} Struct_Data;

typedef struct {
    Struct_Data *e;
    Int cnt;
} Structs;

typedef struct {
    String name;
    Int value;
} Enum_Value;

typedef struct {
    String name;
    String type;
    Bool is_struct;

    Enum_Value *values;
    Int no_of_values;
} Enum_Data;

typedef struct {
    Enum_Data *e;
    Int cnt;
} Enums;

typedef struct {
    String linkage;
    String return_type;
    Int return_type_ptr;
    String name;

    Variable *params;
    Int param_cnt;
} Function_Data;

typedef struct {
    String original;
    String fresh;
} Typedef_Data;

typedef struct {
    Function_Data *e;
    Int cnt;
} Functions;

typedef struct {
    Typedef_Data *e;
    Int cnt;
} Typedefs;

typedef struct {
    Structs structs;
    Enums enums;
    Functions funcs;
    Typedefs typedefs;

    Int struct_max;
    Int enum_max;
    Int func_max;
    Int typedef_max;
} Parse_Result;

Parse_Result parse_streams(Int cnt, Char **fnames);

//
// The following are only exported for test.c
//
typedef struct {
    Char *at;
} Tokenizer;

typedef struct {
    Struct_Data sd;
    Bool success;
} Parse_Struct_Result;
Parse_Struct_Result parse_struct(Tokenizer *tokenizer, Struct_Type struct_type);

#define eat_token(tokenizer) eat_tokens(tokenizer, 1)
Void eat_tokens(Tokenizer *tokenizer, Int num_tokens_to_eat);

File preprocess_macros(File file);

#define _LEXER_H
#endif