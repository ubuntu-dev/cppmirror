/*===================================================================================================
  File:                    utils.cpp
  Author:                  Jonathan Livingstone
  Email:                   seagull127@ymail.com
  Licence:                 Public Domain
                           No Warranty is offered or implied about the reliability,
                           suitability, or usability
                           The use of this code is at your own risk
                           Anyone can use this code, modify it, sell it to terrorists, etc.
  ===================================================================================================*/

#define MAX_POINTER_SIZE 4

//
// Memset and Memcpy
//
Void copy(Void *dest, Void *src, Uintptr size) {
    Byte *dest8 = cast(Byte *)dest;
    Byte *src8 = cast(Byte *)src;

    for(Uintptr i = 0; (i < size); ++i) {
        dest8[i] = src8[i];
    }
}

#define zero(dst, size) set(dst, 0, size)
Void set(void *dest, Byte v, Uintptr n) {
    Byte *dest8 = cast(Byte *)dest;
    for(Uintptr i = 0; (i < n); ++i, ++dest8) {
        *dest8 = cast(Byte)v;
    }
}

//
// Error stuff.
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

    ErrorType_count,
} ErrorType;

typedef struct {
    ErrorType type;
    Char *guid;
} Error;

global Error global_errors[32];
global Int global_error_count;

#define push_error(type) push_error_(type, MAKE_GUID)
Void push_error_(ErrorType type, Char *guid) {
    if(global_error_count + 1 < array_count(global_errors)) {
        Error *e = global_errors + global_error_count++;

        e->type = type;
        e->guid = guid;
    }
}

Int string_length(Char *str);
Char *ErrorTypeToString(ErrorType e) {
    Char *res = 0;

#define ERROR_TYPE_TO_STRING(err) err: { res = #err; } break

    switch(e) {
        case ERROR_TYPE_TO_STRING(ErrorType_ran_out_of_memory);
        case ERROR_TYPE_TO_STRING(ErrorType_assert_failed);
        case ERROR_TYPE_TO_STRING(ErrorType_no_parameters);
        case ERROR_TYPE_TO_STRING(ErrorType_cannot_find_file);
        case ERROR_TYPE_TO_STRING(ErrorType_could_not_write_to_disk);
        case ERROR_TYPE_TO_STRING(ErrorType_could_not_load_file);
        case ERROR_TYPE_TO_STRING(ErrorType_no_files_pass_in);
        case ERROR_TYPE_TO_STRING(ErrorType_could_not_find_mallocd_ptr);
        case ERROR_TYPE_TO_STRING(ErrorType_memory_not_freed);
        case ERROR_TYPE_TO_STRING(ErrorType_could_not_find_struct);
        case ERROR_TYPE_TO_STRING(ErrorType_unknown_token_found);
        case ERROR_TYPE_TO_STRING(ErrorType_failed_to_parse_enum);
        case ERROR_TYPE_TO_STRING(ErrorType_failed_parsing_variable);
        case ERROR_TYPE_TO_STRING(ErrorType_failed_to_find_size_of_array);
        case ERROR_TYPE_TO_STRING(ErrorType_could_not_detect_struct_name);
        case ERROR_TYPE_TO_STRING(ErrorType_did_not_write_entire_file);
        case ERROR_TYPE_TO_STRING(ErrorType_did_not_read_entire_file);
        case ERROR_TYPE_TO_STRING(ErrorType_could_not_create_directory);
        case ERROR_TYPE_TO_STRING(ErrorType_incorrect_number_of_members_for_struct);
        case ERROR_TYPE_TO_STRING(ErrorType_incorrect_struct_name);
        case ERROR_TYPE_TO_STRING(ErrorType_incorrect_number_of_base_structs);
        case ERROR_TYPE_TO_STRING(ErrorType_incorrect_members_in_struct);
        case ERROR_TYPE_TO_STRING(ErrorType_incorrect_data_structure_type);

        default: assert(0); break;
    }

    if(res) {
        Int offset = string_length("ErrorType_");
        res += offset;
    }

#undef ERROR_TYPE_TO_STRING

    return(res);
}

Bool print_errors(void) {
    Bool res = false;

    if(global_error_count) {
        res = true;

        Char buffer2[256] = {0};
        stbsp_snprintf(buffer2, array_count(buffer2), " with %d error(s).\n\n", global_error_count);
        system_write_to_console(buffer2);

        for(Int i = 0; (i < global_error_count); ++i) {
            Char buffer[256] = {0};
            stbsp_snprintf(buffer, array_count(buffer), "%s %s\n",
                           global_errors[i].guid, ErrorTypeToString(global_errors[i].type));
            system_write_to_console(buffer);
        }
    }

    return(res);
}

//
// Temp Memory.
//
Int const default_mem_alignment = 4;

typedef struct {
    Void *e;
    Uintptr size;
    Uintptr used;
} TempMemory;

Uintptr get_alignment(void *mem, Uintptr desired_alignment) {
    Uintptr res = 0;

    Uintptr alignment_mask = desired_alignment - 1;
    if(cast(Uintptr)mem & alignment_mask) {
        res = desired_alignment - (cast(Uintptr)mem & alignment_mask);
    }

    return(res);
}

TempMemory create_temp_buffer(Uintptr size) {
    TempMemory res = {0};

    res.e = system_malloc(size);
    if(res.e) {
        res.size = size;
        zero(res.e, res.size);
    }

    return(res);
}

#define push_type(tm, Type, ...) (Type *)push_size(tm, sizeof(Type), ##__VA_ARGS__)
#define push_size(tm, size) push_size_with_alignment(tm, size, default_mem_alignment)
Void *push_size_with_alignment(TempMemory *tm, Uintptr size, Uintptr alignment) {
    Void *res = 0;

    Uintptr alignment_offset = get_alignment(cast(Byte *)tm->e + tm->used, alignment);

    if(tm->used + alignment_offset < tm->size) {
        res = cast(Byte *)tm->e + tm->used + alignment_offset;
        tm->used += size + alignment_offset;
    } else {
        assert(0);
    }

    return(res);
}

Void free_temp_buffer(TempMemory *temp_memory) {
    system_free(temp_memory->e);
    zero(temp_memory, sizeof(temp_memory));
}

//
// Strings.
//
typedef struct {
    Char *e;
    Uintptr len;
} String;

Int string_length(Char *str) {
    Int res = 0;

    while(*str++) {
        ++res;
    }

    return(res);
}

String create_string(Char *str, Int len) {
    String res;
    res.e = str;
    res.len = (len) ? len : string_length(str);

    return(res);
}

Bool string_concat(Char *dest, Int len, Char *a, Int a_len, Char *b, Int b_len) {
    Bool res = false;

    if(len > a_len + b_len) {
        for(Int i = 0; (i < a_len); ++i) { *dest++ = *a++; }
        for(Int i = 0; (i < b_len); ++i) { *dest++ = *b++; }

        res = true;
    }

    return(res);
}

Bool string_comp_len(Char *a, Char *b, Uintptr len) {
    for(Int i = 0; (i < len); ++i, ++a, ++b) {
        if(*a != *b) {
            return(false);
        }
    }

    return(true);
}

Bool cstring_comp(Char *a, Char *b) {
    while((*a) && (*b)) {
        if(*a != *b) {
            return(false);
        }

        ++a; ++b;
    }

    return(true);
}

Uintptr string_copy(Char *dest, Char *src) {
    Uintptr res = 0;
    while(*src) {
        *dest = *src;
        ++dest;
        ++src;
        ++res; // TODO(Jonny): Could work out at end, rather than increment every time through the loop?
    }

    return(res);
}

Bool string_comp(String a, String b) {
    Bool res = false;

    if(a.len == b.len) {
        res = true;

        for(Int i = 0; (i < a.len); ++i) {
            if(a.e[i] != b.e[i]) {
                res = false;
                break;
            }
        }
    }

    return(res);
}

Bool string_cstring_comp(String a, Char *b) {
    Bool res = true;

    for(Int i = 0; (i < a.len); ++i) {
        if(a.e[i] != b[i]) {
            res = false;
            break;
        }
    }

    return(res);
}

Bool cstring_string_comp(Char *a, String b) {
    Bool res = string_cstring_comp(b, a);

    return(res);
}

Bool string_comp_array(String *a, String *b, Int cnt) {
    Bool res = true;
    for(Int i = 0; (i < cnt); ++i) {
        if(!string_comp(a[i], b[i])) {
            res = false;
            break;
        }
    }

    return(res);
}

Bool string_contains(String str, Char target) {
    Bool res = false;

    for(Int i = 0; (i < str.len); ++i) {
        if(str.e[i] == target) {
            res = true;
            break;
        }
    }

    return(res);
}

Bool cstring_contains(Char *str, Char target) {
    Bool res = false;

    while(*str) {
        if(*str == target) {
            res = true;
            break;
        }

        ++str;
    }

    return(res);
}

Bool string_contains_cstring(String str, Char *target) {
    Int target_len = string_length(target);

    for(Int i = 0; (i < str.len); ++i) {
        if(str.e[i] == target[0]) {
            for(int j = 0; (j < target_len); ++j) {
                if(str.e[i + j] != target[j]) {
                    break;
                }

                if(j == (target_len - 1)) {
                    return(true);
                }
            }
        }
    }

    return(false);
}

Bool cstring_contains_cstring(Char *str, Char *target) {
    String s = create_string(str, string_length(str));
    Bool res = string_contains_cstring(s, target);

    return(res);
}

//
// Stuff
//
typedef struct {
    Int e;
    Bool success;
} ResultInt;

ResultInt char_to_int(Char c) {
    ResultInt res = {0};
    switch(c) {
        case '0': { res.e = 0; res.success = true; } break;
        case '1': { res.e = 1; res.success = true; } break;
        case '2': { res.e = 2; res.success = true; } break;
        case '3': { res.e = 3; res.success = true; } break;
        case '4': { res.e = 4; res.success = true; } break;
        case '5': { res.e = 5; res.success = true; } break;
        case '6': { res.e = 6; res.success = true; } break;
        case '7': { res.e = 7; res.success = true; } break;
        case '8': { res.e = 8; res.success = true; } break;
        case '9': { res.e = 9; res.success = true; } break;
    }

    return(res);
}

ResultInt string_to_int(String str) {
    ResultInt res = {0};

    for(Int i = 0; (i < str.len); ++i) {
        ResultInt temp_int = char_to_int(str.e[i]);
        if(!temp_int.success) {
            break;
        }

        res.e *= 10;
        res.e += temp_int.e;

        if(i == (str.len - 1)) {
            res.success = true;
        }
    }

    return(res);
}

ResultInt cstring_to_int(Char *str) {
    String string;
    string.e = str;
    string.len = string_length(str);
    ResultInt res = string_to_int(string);

    return(res);
}

ResultInt calculator_string_to_int(Char *str) {
    ResultInt res = {0};

    /* TODO(Jonny);
        - Make sure each element in the string is either a number or a operator.
        - Do the calculator in order (multiply, divide, add, subtract).
    */
    String *arr = system_malloc(sizeof(*arr) * 256); // TODO(Jonny): Random size.
    if(arr) {
        Char *at = str;
        arr[0].e = at;
        Int cnt = 0;
        for(; (*at); ++at, ++arr[cnt].len) {
            if(*at == ' ') {
                ++at;
                arr[++cnt].e = at;
            }
        }
        ++cnt;

        Int *nums = system_malloc(sizeof(*nums) * cnt);
        Char *ops = system_malloc(sizeof(*ops) * cnt);
        if((nums) && (ops)) {
            for(Int i = 0, j = 0; (j < cnt); ++i, j += 2) {
                ResultInt r = string_to_int(arr[j]);
                if(r.success) {
                    nums[i] = r.e;
                } else          {
                    goto clean_up;
                }
            }

            for(Int i = 0, j = 1; (j < cnt); ++i, j += 2) {
                assert(arr[j].len == 1);
                ops[i] = *arr[j].e;
            }

            // TODO(Jonny): At this point, I have all the numbers and ops in seperate arrays.

clean_up:;
            system_free(ops);
            system_free(nums);
        }

        system_free(arr);
    }

    return(res);
}

Bool is_in_string_array(String target, String *arr, Int arr_cnt) {
    Bool res = false;
    for(int i = 0; (i < arr_cnt); ++i) {
        if(string_comp(target, arr[i])) {
            res = true;
            break;
        }
    }

    return(res);
}

Uint32 safe_truncate_size_64(Uint64 v) {
    assert(v <= 0xFFFFFFFF);
    Uint32 res = cast(Uint32)v;

    return(res);
}

//
// Variable.
//
typedef enum {
    Access_unknown,
    Access_public,
    Access_private,
    Access_protected,

    Access_count,
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

Variable create_variable(Char *type, Char *name, Int ptr, Int array_count ) {
    Variable res;
    res.type = create_string(type, 0);
    res.name = create_string(name, 0);
    res.ptr = ptr;
    res.array_count = array_count;

    return(res);
}

Bool variable_comp(Variable a, Variable b) {
    Bool res = true;

    if(!string_comp(a.type, b.type))        { res = false; }
    else if(!string_comp(a.name, b.name))   { res = false; }
    else if(a.ptr != b.ptr)                 { res = false; }
    else if(a.array_count != b.array_count) { res = false; }

    return(res);
}

Bool compare_variable_array(Variable *a, Variable *b, Int count) {
    for(Int i = 0; (i < count); ++i) {
        if(!variable_comp(a[i], b[i])) {
            return(false);
        }
    }

    return(true);
}

//
// Utils.
//
#define kilobytes(v) ((v)            * (1024LL))
#define megabytes(v) ((kilobytes(v)) * (1024LL))
#define gigabytes(v) ((megabytes(v)) * (1024LL))

Char to_caps(Char c) {
    Char res = c;
    if((c >= 'a') && (c <= 'z')) {
        res -= 32;
    }

    return(res);
}

Int absolute_value(Int v) {
    Int res = (v > 0) ? v : -v;

    return(res);
}