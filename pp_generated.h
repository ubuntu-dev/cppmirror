#if !defined(PP_GENERATED_H)

#include <stdint.h>

#if !defined(PP_ASSERT)
    #include <assert.h>
    #define PP_ASSERT assert
#endif

#if !defined(PP_SPRINTF)
    #include "stdio.h" 
    #if defined(_MSC_VER)
        #define PP_SPRINTF(buf, size, format, ...) sprintf_s(buf, size, format, ##__VA_ARGS__)
    #else
        #define PP_SPRINTF(buf, size, format, ...) sprintf(buf, format, ##__VA_ARGS__)
    #endif
#endif

#define PP_CONCAT(a, b) a##b
#define PP_TO_STRING(a) #a

#define PP_OFFSETOF(T, member) ((uintptr_t)&(((T *)0)->member))

#if defined(PP_STATIC_FUNCS)
    #define PP_STATIC static
#else
    #define PP_STATIC
#endif

#if defined(_WIN32)
    // TODO(Jonny): Is this specific to MSVC or to Windows?
    #include <intrin.h>
#elif defined(__linux__)
    #include <x86intrin.h>
#endif

// Primitive types.
typedef int32_t pp_MyBool;
#define PP_TRUE 1
#define PP_FALSE 0
typedef char pp_char;
typedef short pp_short;
typedef int pp_int;
typedef long pp_long;
typedef float pp_float;
typedef double pp_double;
typedef uint64_t pp_uint64_t;
typedef uint32_t pp_uint32_t;
typedef uint16_t pp_uint16_t;
typedef uint8_t pp_uint8_t;
typedef int64_t pp_int64_t;
typedef int32_t pp_int32_t;
typedef int16_t pp_int16_t;
typedef int8_t pp_int8_t;
typedef uintptr_t pp_uintptr_t;
typedef intptr_t pp_intptr_t;
typedef size_t pp_size_t;
typedef __m128 pp___m128;
typedef __m128i pp___m128i;

PP_STATIC pp_MyBool pp_string_compare(char const *a, char const *b) {
    for(; (*a != *b); ++a, ++b) {
        if(!(*a) && !(*b)) {
            return(PP_TRUE);
        }
    }

    return(PP_FALSE);
}

#if !defined(PP_MEMSET)
PP_STATIC void *PP_MEMSET(void *dst, uint8_t v, uintptr_t size) {
    uint8_t *dst8 = (uint8_t *)dst;
    while(size-- > 0) {
        *dst8++ = v;
    }

    return(dst);
}
#endif
//
// Forward declared structs, enums, and functions
//
typedef union sglm_V2 sglm_V2;
typedef struct sglm_Mat4x4 sglm_Mat4x4;

//
// An enum, with an index for each type in the codebase.
//
typedef enum pp_Type {
    pp_Type_unknown,

    pp_Type_char,
    pp_Type_short,
    pp_Type_int,
    pp_Type_long,
    pp_Type_float,
    pp_Type_double,
    pp_Type_bool,
    pp_Type_uint64_t,
    pp_Type_uint32_t,
    pp_Type_uint16_t,
    pp_Type_uint8_t,
    pp_Type_int64_t,
    pp_Type_int32_t,
    pp_Type_int16_t,
    pp_Type_int8_t,
    pp_Type_uintptr_t,
    pp_Type_intptr_t,
    pp_Type_size_t,
    pp_Type___m128,
    pp_Type___m128i,
    pp_Type_sglm_Bool,
    pp_Type_sglm_V2,
    pp_Type_sglm_Mat4x4,

    pp_Type_count,
} pp_Type;

//
// Forward declared recreated stuff.
//

// Forward declared structs.
typedef union pp_sglm_V2 pp_sglm_V2;    typedef union pp_sglm_V2 pp_pp_sglm_V2;
typedef struct pp_sglm_Mat4x4 pp_sglm_Mat4x4;    typedef struct pp_sglm_Mat4x4 pp_pp_sglm_Mat4x4;

// Forward declared enums

//
// Create typedefs.
//
typedef pp_int pp_sglm_Bool;

//
// Recreated structs.
//
union pp_sglm_V2 { float e[2];  struct {float x; float y;  }; };
struct pp_sglm_Mat4x4 { __m128 e[4];  };

// Turn a typedef'd type into it's original type.
PP_STATIC pp_Type pp_typedef_to_original(pp_Type type) {
    switch(type) {
        case pp_Type_sglm_Bool: { return(pp_Type_int); } break;
    }

    return(type);
}
typedef struct pp_MemberDefinition {
    pp_Type type;
    char const *name;
    uintptr_t offset;
    uintptr_t ptr;
    uintptr_t arr_size;
} pp_MemberDefinition;

PP_STATIC pp_MemberDefinition pp_get_members_from_type(pp_Type type, uintptr_t index) {
    pp_Type real_type = pp_typedef_to_original(type);
    if(real_type == pp_Type_sglm_V2) {
        switch(index) {
            case 0: {
                pp_MemberDefinition res = {pp_Type_float, "e", PP_OFFSETOF(pp_sglm_V2, e), 0, 2};
                return(res);
            } break; 
            case 1: {
                pp_MemberDefinition res = {pp_Type_float, "x", PP_OFFSETOF(pp_sglm_V2, x), 0, 0};
                return(res);
            } break; 
            case 2: {
                pp_MemberDefinition res = {pp_Type_float, "y", PP_OFFSETOF(pp_sglm_V2, y), 0, 0};
                return(res);
            } break; 
        }
    }
    else if(real_type == pp_Type_sglm_Mat4x4) {
        switch(index) {
            case 0: {
                pp_MemberDefinition res = {pp_Type___m128, "e", PP_OFFSETOF(pp_sglm_Mat4x4, e), 0, 4};
                return(res);
            } break; 
        }
    }

    // Not found
    PP_ASSERT(0);
    pp_MemberDefinition failres; PP_MEMSET(&failres, 0, sizeof(failres)); // Zero all the results for failure case.
    return(failres);
}

PP_STATIC uintptr_t pp_get_number_of_members(pp_Type type) {
    switch(pp_typedef_to_original(type)) {
        case pp_Type_sglm_V2: { return(3); } break;
        case pp_Type_sglm_Mat4x4: { return(1); } break;
    }

    PP_ASSERT(0);
    return(0);
}

typedef enum pp_StructureType {
    pp_StructureType_unknown,

    pp_StructureType_primitive,
    pp_StructureType_struct,
    pp_StructureType_enum,

    pp_StructureType_count,
} pp_StructureType;

PP_STATIC pp_StructureType pp_get_structure_type(pp_Type type) {
    switch(pp_typedef_to_original(type)) {
        case pp_Type_char: case pp_Type_short: case pp_Type_int: case pp_Type_long: case pp_Type_float: case pp_Type_double: case pp_Type_bool: case pp_Type_uint64_t: case pp_Type_uint32_t: case pp_Type_uint16_t: case pp_Type_uint8_t: case pp_Type_int64_t: case pp_Type_int32_t: case pp_Type_int16_t: case pp_Type_int8_t: case pp_Type_uintptr_t: case pp_Type_intptr_t: case pp_Type_size_t: case pp_Type___m128: case pp_Type___m128i: {
            return(pp_StructureType_primitive);
        } break;

        case pp_Type_sglm_V2: case pp_Type_sglm_Mat4x4: {
            return(pp_StructureType_struct);
        } break;
    }

    PP_ASSERT(0);
    return(pp_StructureType_unknown);
}

PP_STATIC char const * pp_type_to_string(pp_Type type) {
    switch(type) {
        case pp_Type_char: { return("char"); } break;
        case pp_Type_short: { return("short"); } break;
        case pp_Type_int: { return("int"); } break;
        case pp_Type_long: { return("long"); } break;
        case pp_Type_float: { return("float"); } break;
        case pp_Type_double: { return("double"); } break;
        case pp_Type_bool: { return("bool"); } break;
        case pp_Type_uint64_t: { return("uint64_t"); } break;
        case pp_Type_uint32_t: { return("uint32_t"); } break;
        case pp_Type_uint16_t: { return("uint16_t"); } break;
        case pp_Type_uint8_t: { return("uint8_t"); } break;
        case pp_Type_int64_t: { return("int64_t"); } break;
        case pp_Type_int32_t: { return("int32_t"); } break;
        case pp_Type_int16_t: { return("int16_t"); } break;
        case pp_Type_int8_t: { return("int8_t"); } break;
        case pp_Type_uintptr_t: { return("uintptr_t"); } break;
        case pp_Type_intptr_t: { return("intptr_t"); } break;
        case pp_Type_size_t: { return("size_t"); } break;
        case pp_Type___m128: { return("__m128"); } break;
        case pp_Type___m128i: { return("__m128i"); } break;
        case pp_Type_sglm_V2: { return("sglm_V2"); } break;
        case pp_Type_sglm_Mat4x4: { return("sglm_Mat4x4"); } break;
        case pp_Type_sglm_Bool: { return("sglm_Bool"); } break;
    }
    
    PP_ASSERT(0);
    return(0);
}

PP_STATIC uintptr_t pp_get_size_from_type(pp_Type type) {
    switch(pp_typedef_to_original(type)) {
        // Primitives
        case pp_Type_char: { return(sizeof(pp_char)); } break;
        case pp_Type_short: { return(sizeof(pp_short)); } break;
        case pp_Type_int: { return(sizeof(pp_int)); } break;
        case pp_Type_long: { return(sizeof(pp_long)); } break;
        case pp_Type_float: { return(sizeof(pp_float)); } break;
        case pp_Type_double: { return(sizeof(pp_double)); } break;
        case pp_Type_uint64_t: { return(sizeof(pp_uint64_t)); } break;
        case pp_Type_uint32_t: { return(sizeof(pp_uint32_t)); } break;
        case pp_Type_uint16_t: { return(sizeof(pp_uint16_t)); } break;
        case pp_Type_uint8_t: { return(sizeof(pp_uint8_t)); } break;
        case pp_Type_int64_t: { return(sizeof(pp_int64_t)); } break;
        case pp_Type_int32_t: { return(sizeof(pp_int32_t)); } break;
        case pp_Type_int16_t: { return(sizeof(pp_int16_t)); } break;
        case pp_Type_int8_t: { return(sizeof(pp_int8_t)); } break;
        case pp_Type_uintptr_t: { return(sizeof(pp_uintptr_t)); } break;
        case pp_Type_intptr_t: { return(sizeof(pp_intptr_t)); } break;
        case pp_Type_size_t: { return(sizeof(pp_size_t)); } break;
        case pp_Type___m128: { return(sizeof(pp___m128)); } break;
        case pp_Type___m128i: { return(sizeof(pp___m128i)); } break;

        // Structs
        case pp_Type_sglm_V2: { return(sizeof(pp_sglm_V2)); } break;
        case pp_Type_sglm_Mat4x4: { return(sizeof(pp_sglm_Mat4x4)); } break;
    }

    PP_ASSERT(0);
    return(0);
}
PP_STATIC char const * pp_enum_to_string(pp_Type type, intptr_t index);
// uintptr_t pp_serialize_struct(TYPE *var, TYPE, buffer, buffer_size);
#define pp_serialize_struct(var, Type, buf, size) pp_serialize_struct_(var, PP_CONCAT(pp_Type_, Type), PP_TO_STRING(var), 0, buf, size, 0)
PP_STATIC uintptr_t
pp_serialize_struct_(void *var, pp_Type type, char const *name, uintptr_t indent, char *buffer, uintptr_t buf_size, uintptr_t bytes_written) {
    char indent_buf[256] = {0};
    PP_ASSERT((buffer) && (buf_size > 0)); // Check params.

    // Setup the indent buffer.
    indent += 4;
    for(uintptr_t i = 0; (i < indent); ++i) {indent_buf[i] = ' ';}

    uintptr_t num_members = pp_get_number_of_members(type); PP_ASSERT(num_members != -1); // Get the number of members for the for loop.
    for(uintptr_t i = 0; (i < num_members); ++i) {
        pp_MemberDefinition member = pp_get_members_from_type(type, i);
        uintptr_t *member_ptr = (uintptr_t *)((char *)var + member.offset); // Get the actual pointer to the memory address.
        pp_StructureType struct_type = pp_get_structure_type(member.type);
        if(struct_type == pp_StructureType_primitive) {
            char const *type_as_string = pp_type_to_string(member.type);
            if(member.arr_size > 1) {
                for(int j = 0; (j < member.arr_size); ++j) {
                    uintptr_t *member_ptr_as_uintptr = (uintptr_t *)member_ptr; // For arrays of pointers.
                    pp_MyBool is_null = (member.ptr) ? member_ptr_as_uintptr[j] == 0 : PP_FALSE;
                    if(!is_null) {

#define print_prim_arr(m, Type, p) Type *v = (member.ptr) ? *(Type **)((char unsigned *)member_ptr + (sizeof(uintptr_t) * j)) : &((Type *)member_ptr)[j]; bytes_written += PP_SPRINTF((char *)buffer + bytes_written, buf_size - bytes_written, "\n%s " #Type " %s%s[%d] = " m "", indent_buf, (member.ptr) ? "*" : "", member.name, j, p (Type *)v)
                        pp_Type original_type = pp_typedef_to_original(member.type);
                        if(original_type == pp_Type_double)     { print_prim_arr("%f",  double, *); }
                        else if(original_type == pp_Type_float) { print_prim_arr("%f",  float, *);  }
                        else if(original_type == pp_Type_int)   { print_prim_arr("%d",  int, *);    }
                        else if(original_type == pp_Type_long)  { print_prim_arr("%ld", long, *);   }
                        else if(original_type == pp_Type_short) { print_prim_arr("%d",  short, *);  }
                        else if(original_type == pp_Type_bool)  { print_prim_arr("%d",  int, *);    }

                        else if(original_type == pp_Type_uint64_t) { print_prim_arr("%u", uint64_t, *); }
                        else if(original_type == pp_Type_int64_t)  { print_prim_arr("%d", int64_t, *);  }
                        else if(original_type == pp_Type_uint32_t) { print_prim_arr("%u", uint32_t, *); }
                        else if(original_type == pp_Type_int32_t)  { print_prim_arr("%d", int32_t, *);  }
                        else if(original_type == pp_Type_uint16_t) { print_prim_arr("%u", uint16_t, *); }
                        else if(original_type == pp_Type_int16_t)  { print_prim_arr("%d", int16_t, *);  }
                        else if(original_type == pp_Type_uint8_t) { print_prim_arr("%u", uint8_t, *); }
                        else if(original_type == pp_Type_int8_t)  { print_prim_arr("%d", int8_t, *);  }

                        else if(original_type == pp_Type_intptr_t)  { print_prim_arr("%d", intptr_t, *);  }
                        else if(original_type == pp_Type_uintptr_t) { print_prim_arr("%u", uintptr_t, *); }
                        else if(original_type == pp_Type_size_t)    { print_prim_arr("%u", size_t, *);    }

                        else if(original_type == pp_Type_char) {
                            if(member.ptr) {
                                print_prim_arr("%s",  char, /**/);
                            } else {
                                print_prim_arr("%c",  char, *);
                            }
                        }
#undef print_prim_arr
                    } else {
                        bytes_written += PP_SPRINTF((char *)buffer + bytes_written, buf_size - bytes_written, "\n%s %s %s%s[%d] = (null)", indent_buf, type_as_string, (member.ptr) ? "*" : "", member.name, j);
                    }
                }
            } else {
                uintptr_t *v;
                if(member.ptr) {
                    v = *(uintptr_t **)member_ptr;
                    for(uintptr_t k = 0; (k < member.ptr - 1); ++k) {
                        v = *(uintptr_t **)v;
                    }
                } else {
                    v = (uintptr_t *)member_ptr;
                }
                if(v) {
#define print_prim(m, Type, p) bytes_written += PP_SPRINTF((char *)buffer + bytes_written, buf_size - bytes_written, "\n%s " #Type " %s%s = " m "", indent_buf, (member.ptr) ? "*" : "", member.name, p (Type *)v)
                    pp_Type original_type = pp_typedef_to_original(member.type);
                    if(original_type == pp_Type_double)     { print_prim("%f",  double, *); }
                    else if(original_type == pp_Type_float) { print_prim("%f",  float, *);  }
                    else if(original_type == pp_Type_int)   { print_prim("%d",  int, *);    }
                    else if(original_type == pp_Type_long)  { print_prim("%ld", long, *);   }
                    else if(original_type == pp_Type_short) { print_prim("%d",  short, *);  }
                    else if(original_type == pp_Type_bool)  { print_prim("%d",  int, *);    }

                    else if(original_type == pp_Type_uint64_t)  { print_prim("%u", uint64_t, *); }
                    else if(original_type == pp_Type_int64_t)   { print_prim("%d", int64_t, *);  }
                    else if(original_type == pp_Type_uint32_t)  { print_prim("%u", uint32_t, *); }
                    else if(original_type == pp_Type_int32_t)   { print_prim("%d", int32_t, *);  }
                    else if(original_type == pp_Type_uint16_t)  { print_prim("%u", uint16_t, *); }
                    else if(original_type == pp_Type_int16_t)   { print_prim("%d", int16_t, *);  }
                    else if(original_type == pp_Type_uint8_t)   { print_prim("%u", uint8_t, *);  }
                    else if(original_type == pp_Type_int8_t)    { print_prim("%d", int8_t, *);   }

                    else if(original_type == pp_Type_intptr_t)  { print_prim("%d", intptr_t, *);  }
                    else if(original_type == pp_Type_uintptr_t) { print_prim("%u", uintptr_t, *); }
                    else if(original_type == pp_Type_size_t)    { print_prim("%u", size_t, *);    }

                    else if(original_type == pp_Type_char) {
                        if(member.ptr) {
                            print_prim("%s",  char, /**/);
                        } else {
                            print_prim("%c",  char, *);
                        }
                    }
#undef print_prim
                } else {
                    bytes_written += PP_SPRINTF((char *)buffer + bytes_written, buf_size - bytes_written, "\n%s %s *%s = (null)", indent_buf, type_as_string, member.name);
                }
            }
        } else if(struct_type == pp_StructureType_enum) {
            char const *type_as_string = pp_type_to_string(member.type);

            int *v = 0; // Assuming the enum is an integer.
            if(member.ptr) {
                v = *(int **)member_ptr;
                for(int k = 0; (k < member.ptr - 1); ++k) {
                    v = *(int **)v;
                }
            } else {
                v= (int *)member_ptr;
            }

            pp_Type original_type = pp_typedef_to_original(member.type);
            char const *enum_as_string = pp_enum_to_string(original_type, *(int *)v);
            if(enum_as_string) {
                bytes_written += PP_SPRINTF((char *)buffer + bytes_written, buf_size - bytes_written, "\n%s %s *%s = %s", indent_buf, type_as_string, member.name, enum_as_string);
            }

        } else if(struct_type == pp_StructureType_struct) {
            char tmp_arr_buf[64] = {0};
            if(member.arr_size > 1) {
                PP_SPRINTF(tmp_arr_buf, 32, "[%d]", (int)member.arr_size);
            }

            bytes_written += PP_SPRINTF((char *)buffer + bytes_written, buf_size - bytes_written, "\n%s%s %s%s%s", indent_buf, pp_type_to_string(member.type), (member.ptr) ? "*" : "",member.name, tmp_arr_buf);
            if(member.arr_size <= 1) {
                void *ptr = (member.ptr) ? *(uintptr_t **)member_ptr : member_ptr;
                bytes_written = pp_serialize_struct_(ptr, member.type, member.name, indent, buffer, buf_size - bytes_written, bytes_written);
            } else {
                for(uintptr_t j = 0; (j < member.arr_size); ++j) {
                    uintptr_t size_of_struct = pp_get_size_from_type(member.type);

                    char unsigned *ptr;
                    if(member.ptr) {
                        ptr = *(char unsigned **)((char unsigned *)member_ptr + (j * sizeof(uintptr_t)));
                    } else {
                        ptr = ((char unsigned *)member_ptr + (j * size_of_struct));
                    }

                    bytes_written = pp_serialize_struct_(ptr, member.type, member.name, indent, buffer, buf_size - bytes_written, bytes_written);
                }
            }
        }
    }

    buffer[bytes_written] = 0;
    return(bytes_written);
}

//
// Number of members in an enum.
//
PP_STATIC uintptr_t pp_get_enum_size(pp_Type type) {

    PP_ASSERT(0);
    return(0);
}

//
// String to enum.
//
PP_STATIC intptr_t pp_string_to_enum(pp_Type type, char const *str) {

    PP_ASSERT(0);
    return(0);
}

//
// Enum to string.
//
PP_STATIC char const * pp_enum_to_string(pp_Type type, intptr_t index) {

    PP_ASSERT(0);
    return(0);
}

#define PP_GENERATED_H
#endif // #if defined(PP_GENERATED_H)

