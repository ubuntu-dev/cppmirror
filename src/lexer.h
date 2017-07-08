#if !defined(_LEXER_H)

#include "types.h"
#include "utilities.h"

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
} StructType;

typedef struct {
    String template_header;
    String name;
    Int member_count;
    Variable *members;

    Int inherited_count;
    String *inherited;

    StructType struct_type;
} StructData;

typedef struct {
    StructData *e;
    Int cnt;
} Structs;

typedef struct {
    String name;
    Int value;
} EnumValue;

typedef struct {
    String name;
    String type;
    Bool is_struct;

    EnumValue *values;
    Int no_of_values;
} EnumData;

typedef struct {
    EnumData *e;
    Int cnt;
} Enums;

typedef struct {
    String linkage;
    String return_type;
    Int return_type_ptr;
    String name;

    Variable *params;
    Int param_cnt;
} FunctionData;

typedef struct {
    String original;
    String fresh;
} TypedefData;

typedef struct {
    FunctionData *e;
    Int cnt;
} Functions;

typedef struct {
    TypedefData *e;
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
} ParseResult;

ParseResult parse_streams(Int cnt, Char **fnames);

//
// The following are only exported for test.c
//
typedef struct Tokenizer {
    Char *at;
} Tokenizer;

typedef struct {
    StructData sd;
    Bool success;
} ParseStructResult;
ParseStructResult parse_struct(Tokenizer *tokenizer, StructType struct_type);

#define eat_token(tokenizer) eat_tokens(tokenizer, 1)
Void eat_tokens(Tokenizer *tokenizer, Int num_tokens_to_eat);


#define _LEXER_H
#endif