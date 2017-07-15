#if !defined(_UTILS_H)

#include "types.h"

#define MAX_POINTER_SIZE 4

//
// Error.
//
#if COMPILER_MSVC
    #define GUID__(file, seperator, line) file seperator line ")"
    #define GUID_(file, line) GUID__(file, "(", #line)
    #define GUID(file, line) GUID_(file, line)
    #define MAKE_GUID GUID(__FILE__, __LINE__)
#else
    #define GUID__(file, seperator, line) file seperator line ":1: error:"
    #define GUID_(file, line) GUID__(file, ":", #line)
    #define GUID(file, line) GUID_(file, line)
    #define MAKE_GUID GUID(__FILE__, __LINE__)
#endif

typedef enum {
    ErrorType_unknown,

    ErrorType_ran_out_of_memory,
    ErrorType_assert_failed,
    ErrorType_no_parameters,
    ErrorType_cannot_find_file,
    ErrorType_could_not_write_to_disk,
    ErrorType_could_not_load_file,
    ErrorType_no_files_pass_in,
    ErrorType_could_not_find_mallocd_ptr,
    ErrorType_memory_not_freed,
    ErrorType_could_not_detect_struct_name,
    ErrorType_could_not_find_struct,
    ErrorType_unknown_token_found,
    ErrorType_failed_to_parse_enum,
    ErrorType_failed_parsing_variable,
    ErrorType_failed_to_find_size_of_array,
    ErrorType_did_not_write_entire_file,
    ErrorType_did_not_read_entire_file,
    ErrorType_could_not_create_directory,

    ErrorType_incorrect_number_of_members_for_struct,
    ErrorType_incorrect_struct_name,
    ErrorType_incorrect_number_of_base_structs,
    ErrorType_incorrect_members_in_struct,
    ErrorType_incorrect_data_structure_type,

    ErrorType_count
} ErrorType;

#define push_error(type) push_error_(type, MAKE_GUID)
Void push_error_(ErrorType type, Char *guid);

Bool print_errors(void);

//
// String.
//
typedef struct {
    Char *e;
    Uintptr len;
} String;

String create_string(Char *str, Int len);
Int string_length(Char *str);
Bool string_concat(Char *dest, Int len, Char *a, Int a_len, Char *b, Int b_len);

Uintptr string_copy(Char *dest, Char *src);

Bool cstring_comp(Char *a, Char *b);
Bool string_comp_len(Char *a, Char *b, Uintptr len);
Bool string_comp(String a, String b);
Bool string_cstring_comp(String a, Char *b);
Bool cstring_string_comp(Char *a, String b);
Bool string_comp_array(String *a, String *b, Int cnt);

Bool string_contains(String str, Char target);
Bool cstring_contains(Char *str, Char target);
Bool string_contains_cstring(String str, Char *target);
Bool cstring_contains_cstring(Char *str, Char *target);

//
// Memory.
//

Void copy(Void *dest, Void *src, Uintptr size);

#define zero(dst, size) set(dst, 0, size)
Void set(Void *dest, Byte v, Uintptr n);


typedef struct {
    Byte *e;
    Uintptr size;
    Uintptr used;
} TempMemory;

Uintptr get_alignment(void *mem, Uintptr desired_alignment);
TempMemory create_temp_buffer(Uintptr size);

#define push_size(tm, size) push_size_with_alignment(tm, size, -1)
Void *push_size_with_alignment(TempMemory *tm, Uintptr size, Int alignment);
Void free_temp_buffer(TempMemory *temp_memory);

//
// Variable.
//
typedef enum {
    Access_unknown,
    Access_public,
    Access_private,
    Access_protected,

    Access_count
} Access;

typedef struct {
    String type;
    String name;

    Access access;

    Int ptr;
    Int array_count;

    Uint32 modifier;

    Bool is_inside_anonymous_struct;
} Variable;

Variable create_variable(Char *type, Char *name, Int ptr, Int array_count);
Bool variable_comp(Variable a, Variable b);

//
// Stuff
//
#define kilobytes(v) ((v)            * (1024LL))
#define megabytes(v) ((kilobytes(v)) * (1024LL))
#define gigabytes(v) ((megabytes(v)) * (1024LL))

typedef struct {
    Int e;
    Bool success;
} ResultInt;
ResultInt string_to_int(String str);
ResultInt cstring_to_int(Char *str);

Uint32 safe_truncate_size_64(Uint64 v);

Int absolute_value(Int v);

#define _UTILS_H
#endif