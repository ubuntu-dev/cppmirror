/*===================================================================================================
  File:                    write_file.cpp
  Author:                  Jonathan Livingstone
  Email:                   seagull127@ymail.com
  Licence:                 Public Domain
                           No Warranty is offered or implied about the reliability,
                           suitability, or usability
                           The use of this code is at your own risk
                           Anyone can use this code, modify it, sell it to terrorists, etc.
  ===================================================================================================*/

#include "write_file.h"
#include "lexer.h"
#include "stb_sprintf.h"
#include "platform.h"

struct OutputBuffer {
    Char *buffer;
    Int index;
    Int size;
};

enum StdTypes {
    StdTypes_not,
    StdTypes_vector,
    StdTypes_deque,
    StdTypes_forward_list,
    StdTypes_list,
    StdTypes_string,

    StdTypes_cnt,
};

struct StdResult {
    StdTypes type;
    String stored_type;
};

internal Void write_to_output_buffer(OutputBuffer *ob, Char *format, ...) {
    va_list args;
    va_start(args, format);
    ob->index += stbsp_vsnprintf(ob->buffer + ob->index, ob->size - ob->index, format, args);
    assert(ob->index < ob->size);
    va_end(args);
}

internal Char *primitive_types[] = {"char", "short", "int", "long", "float", "double", "bool"};
#define get_num_of_primitive_types() array_count(primitive_types)

internal Int set_primitive_type(String *array) {
    Int res = array_count(primitive_types);

    for(Int i = 0; (i < res); ++i) {
        String *index = array + i;

        index->e = primitive_types[i];
        index->len = string_length(primitive_types[i]);
    }

    return(res);
}

internal StructData *find_struct(String str, Structs structs) {
    StructData *res = 0;

    if(str.len) {
        for(Int i = 0; (i < structs.cnt); ++i) {
            StructData *sd = structs.e + i;

            if(string_compare(str, sd->name)) {
                res = sd;
                break; // for
            }
        }
    }

    return(res);
}

internal Bool is_meta_type_already_in_array(String *array, Int len, String test) {
    Bool res = false;

    for(Int i = 0; (i < len); ++i) {
        if(string_compare(array[i], test)) {
            res = true;
            break; // for
        }
    }

    return(res);
}

internal Int get_actual_type_count(String *types, Structs structs) {
    Int res = set_primitive_type(types);

    // Fill out the enum meta type enum.
    for(Int i = 0; (i < structs.cnt); ++i) {
        StructData *sd = structs.e + i;

        if(!is_meta_type_already_in_array(types, res, sd->name)) {
            types[res++] = sd->name;
        }

        for(Int j = 0; (j < sd->member_count); ++j) {
            Variable *md = sd->members + j;

            if(!is_meta_type_already_in_array(types, res, md->type)) {
                types[res++] = md->type;
            }
        }
    }

    return(res);
}

enum DataType {
    DataType_unknown,
    DataType_primitive,
    DataType_struct,
    DataType_enum,

    DataType_count,
};

internal Void write_type_info(OutputBuffer *ob, DataType type, String name, String base = create_string("void"),
                              Int member_count = 0, Int inherited_count = 0) {
    write_to_output_buffer(ob, "\n// struct %.*s\n", name.len, name.e);

    Bool alternate_ref = false;
    for(Int i = 0; (i < max_ptr_size); ++i) {
        char ptr_buf[max_ptr_size + 1] = {};
        for(Int j = 0; (j < i); ++j) {
            ptr_buf[j] = '*';
        }

        for(Int j = 0; (j < 2); ++j) {
            if((!j) || (!string_compare(name, "void"))) {
                Char const *ref = (j) ? "&" : "";

                Int ptr_count = 0;
                Char *at = ptr_buf;
                while(*at) {
                    if(*at == '*') {
                        ++ptr_count;
                    }

                    at++;
                }

                write_to_output_buffer(ob,
                                       "template<> struct pp_TypeInfo<%.*s%s%s> {\n"
                                       "    using type      = %.*s%s%s;\n"
                                       "    using weak_type = %.*s;\n"
                                       "    using base      = %.*s;\n"
                                       "\n"
                                       "    static char const * const name;\n"
                                       "    static char const * const weak_name;\n"
                                       "\n"
                                       "    static size_t const member_count = %d;\n"
                                       "    static size_t const base_count   = %d;\n"
                                       "\n"
                                       "    static size_t const ptr_level = %d;\n"
                                       "    static bool   const is_ref    = %s;\n"
                                       "\n"
                                       "    static bool const is_primitive = %s;\n"
                                       "    static bool const is_struct    = %s;\n"
                                       "    static bool const is_enum      = %s;\n"
                                       // TODO(Jonny): Could I have an "is_functor" too? Or is that stupid?
                                       "};\n"
                                       "char const * const pp_TypeInfo<%.*s%s%s>::name      = \"%.*s%s%s\";\n"
                                       "char const * const pp_TypeInfo<%.*s%s%s>::weak_name = \"%.*s\";\n"
                                       "\n",
                                       name.len, name.e, ptr_buf, ref,
                                       name.len, name.e, ptr_buf, ref,
                                       name.len, name.e,
                                       base.len, base.e,
                                       member_count,
                                       inherited_count,
                                       ptr_count,
                                       (j) ? "true" : "false",
                                       (type == DataType_primitive) ? "true" : "false",
                                       (type == DataType_struct) ? "true" : "false", // TODO(Jonny): Should I set this to true for enum classes?
                                       (type == DataType_enum) ? "true" : "false",
                                       name.len, name.e, ptr_buf, ref,
                                       name.len, name.e, ptr_buf, ref,
                                       name.len, name.e, ptr_buf, ref,
                                       name.len, name.e);
            }
        }
    }
}

internal Void write_out_type_specification_struct(OutputBuffer *ob, Structs structs, Enums enums) {
    PtrSize size = 128;
    String *written_members = system_alloc(String, size);
    Int member_cnt = 0;

    for(Int i = 0; (i < enums.cnt); ++i) {
        EnumData *ed = enums.e + i;

        if(member_cnt >= size) {
            size *= 2;
            void *ptr = system_realloc(written_members, sizeof(String) * size);
            if(ptr) {
                written_members = cast(String *)ptr;
            }
        }

        written_members[member_cnt++] = ed->name;
    }

    write_to_output_buffer(ob,
                           "\n\n"
                           "//\n"
                           "// Meta type specialization\n"
                           "//\n");

    write_type_info(ob, DataType_primitive, create_string("void"));

    String primatives[array_count(primitive_types)] = {};
    set_primitive_type(primatives);

    for(Int i = 0; (i < array_count(primatives)); ++i) {
        if(!is_in_string_array(primatives[i], written_members, member_cnt)) {
            if(member_cnt >= size) {
                size *= 2;
                void *ptr = system_realloc(written_members, sizeof(String) * size);
                if(ptr) {
                    written_members = cast(String *)ptr;
                }
            }

            written_members[member_cnt++] = primatives[i];

            write_type_info(ob, DataType_primitive, primatives[i]);
        }
    }

    for(Int i = 0; (i < structs.cnt); ++i) {
        StructData *sd = structs.e + i;

        if(!is_in_string_array(sd->name, written_members, member_cnt)) {
            if(member_cnt >= size) {
                size *= 2;
                void *ptr = system_realloc(written_members, sizeof(String) * size);
                if(ptr) {
                    written_members = cast(String *)ptr;
                }
            }

            written_members[member_cnt++] = sd->name;

            write_type_info(ob, DataType_struct, sd->name, (sd->inherited_count) ? sd->inherited[0] : create_string("void"),
                            sd->member_count, sd->inherited_count);

            for(Int j = 0; (j < sd->member_count); ++j) {
                Variable *md = sd->members + j;

                if(!is_in_string_array(md->type, written_members, member_cnt)) {
                    if(member_cnt >= size) {
                        size *= 2;
                        void *ptr = system_realloc(written_members, sizeof(String) * size);
                        if(ptr) {
                            written_members = cast(String *)ptr;
                        }
                    }

                    written_members[member_cnt++] = md->type;

                    Int number_of_members = 0;
                    StructData *members_struct_data = find_struct(md->type, structs);
                    if(members_struct_data) {
                        number_of_members = members_struct_data->member_count;
                    }

                    StructData *member_data = find_struct(md->type, structs);
                    if(member_data) {
                        write_type_info(ob, DataType_struct, member_data->name,
                                        (member_data->inherited_count) ? member_data->inherited[0] : create_string("void"),
                                        member_data->member_count, member_data->inherited_count);
                    }
                }
            }
        }
    }

    system_free( written_members);
}

File write_data(ParseResult pr) {
    File res = {};

    OutputBuffer ob = {};
    ob.size = 1024 * 1024;
    ob.buffer = system_alloc(Char, ob.size);
    if(ob.buffer) {
        write_to_output_buffer(&ob,
                               "#if !defined(PP_GENERATED_H)\n"
                               "\n"
                               "//\n"
                               "// Detect compiler.\n"
                               "//\n"
                               "#if defined(PP_COMPILER_MSVC)\n"
                               "    #undef PP_COMPILER_MSVC\n"
                               "    #undef PP_COMPILER_CLANG\n"
                               "    #undef PP_COMPILER_GCC\n"
                               "\n"
                               "    #define PP_COMPILER_MSVC 1\n"
                               "    #define PP_COMPILER_CLANG 0\n"
                               "    #define PP_COMPILER_GCC 0\n"
                               "#elif defined(PP_COMPILER_CLANG)\n"
                               "    #undef PP_COMPILER_MSVC\n"
                               "    #undef PP_COMPILER_CLANG\n"
                               "    #undef PP_COMPILER_GCC\n"
                               "\n"
                               "    #define PP_COMPILER_MSVC 0\n"
                               "    #define PP_COMPILER_CLANG 1\n"
                               "    #define PP_COMPILER_GCC 0\n"
                               "#elif defined(PP_COMPILER_GCC)\n"
                               "    #undef PP_COMPILER_MSVC\n"
                               "    #undef PP_COMPILER_CLANG\n"
                               "    #undef PP_COMPILER_GCC\n"
                               "\n"
                               "    #define PP_COMPILER_MSVC 0\n"
                               "    #define PP_COMPILER_CLANG 0\n"
                               "    #define PP_COMPILER_GCC 1\n"
                               "#else\n"
                               "    #define PP_COMPILER_MSVC 0\n"
                               "    #define PP_COMPILER_CLANG 0\n"
                               "    #define PP_COMPILER_GCC 0\n"
                               "\n"
                               "    #if defined(__clang__)\n"
                               "        #undef PP_COMPILER_CLANG\n"
                               "        #define PP_COMPILER_CLANG 1\n"
                               "    #elif defined(_MSC_VER)\n"
                               "        #undef PP_COMPILER_MSVC\n"
                               "        #define PP_COMPILER_MSVC 1\n"
                               "    #elif (defined(__GNUC__) || defined(__GNUG__)) // This has to be after __clang__, because Clang also defines this.\n"
                               "        #undef PP_COMPILER_GCC\n"
                               "        #define PP_COMPILER_GCC 1\n"
                               "    #endif\n"
                               "#endif\n"
                               "\n"
                               "//\n"
                               "// Detect OS.\n"
                               "//\n"
                               "#if defined(PP_OS_WIN32)\n"
                               "    #undef PP_OS_WIN32\n"
                               "    #undef PP_OS_LINUX\n"
                               "\n"
                               "    #define PP_OS_WIN32 1\n"
                               "    #define PP_OS_LINUX 0\n"
                               "#elif defined(PP_OS_LINUX)\n"
                               "    #undef PP_OS_WIN32\n"
                               "    #undef PP_OS_LINUX\n"
                               "\n"
                               "    #define PP_OS_WIN32 0\n"
                               "    #define PP_OS_LINUX 1\n"
                               "#else"
                               "    #define PP_OS_WIN32 0\n"
                               "    #define PP_OS_LINUX 0\n"
                               "\n"
                               "    #if defined(__linux__)\n"
                               "        #define PP_OS_LINUX 1\n"
                               "    #elif defined(_WIN32)\n"
                               "        #define PP_OS_WIN32 1\n"
                               "    #endif\n"
                               "#endif\n"
                               "\n"
                               "//\n"
                               "// Detect 32/64 bit.\n"
                               "//\n"
                               "#if defined(PP_ENVIRONMENT64)\n"
                               "    #undef PP_ENVIRONMENT64\n"
                               "    #undef PP_ENVIRONMENT32\n"
                               "\n"
                               "    #define PP_ENVIRONMENT64 1\n"
                               "    #define PP_ENVIRONMENT32 0\n"
                               "#elif defined(PP_ENVIRONMENT32)\n"
                               "    #undef PP_ENVIRONMENT64\n"
                               "    #undef PP_ENVIRONMENT32\n"
                               "\n"
                               "    #define PP_ENVIRONMENT64 0\n"
                               "    #define PP_ENVIRONMENT32 1\n"
                               "#else"
                               "    #define PP_ENVIRONMENT64 0\n"
                               "    #define PP_ENVIRONMENT32 0\n"
                               "\n"
                               "    #if PP_OS_LINUX\n"
                               "        #if (__x86_64__ || __ppc64__)\n"
                               "            #undef PP_ENVIRONMENT64\n"
                               "            #define PP_ENVIRONMENT64 1\n"
                               "        #else\n"
                               "            #undef PP_ENVIRONMENT32\n"
                               "            #define PP_ENVIRONMENT32 1\n"
                               "        #endif\n"
                               "    #elif OS_WIN32\n"
                               "        #if defined(_WIN64)\n"
                               "            #undef PP_ENVIRONMENT64\n"
                               "            #define PP_ENVIRONMENT64 1\n"
                               "        #else\n"
                               "            #undef PP_ENVIRONMENT32\n"
                               "            #define PP_ENVIRONMENT32 1\n"
                               "        #endif\n"
                               "    #endif\n"
                               "#endif\n"
                               "\n"
                               "typedef void pp_void;\n"
                               "typedef char pp_char;\n"
                               "typedef short pp_short;\n"
                               "typedef int pp_int;\n"
                               "typedef long pp_long;\n"
                               "typedef float pp_float;\n"
                               "typedef double pp_double;\n"
                               "typedef bool pp_bool;\n"
                               "typedef size_t pp_size_t;\n"
                               "\n"
                               "#define pp_std std\n"
                               "\n"
                               "//\n"
                               "// Type compare and weak type compare.\n"
                               "//\n"
                               "template<class T, class U>struct pp_TypeCompare_{ static bool const e = false; };\n"
                               "template<class T>struct pp_TypeCompare_<T, T>{ static bool const e = true; };\n"
                               "#define pp_type_compare(A, B) pp_TypeCompare_<A, B>::e\n"
                               "#define pp_weak_type_compare(A, B) pp_TypeCompare_<pp::TypeInfo<A>::weak_type, pp::TypeInfo<B>::weak_type>::e;\n"
                               "\n"
                               "static bool pp_string_compare(char const *a, char const *b) {\n"
                               "    for(;; ++a, ++b) {\n"
                               "        if((*a == 0) && (*b == 0)) {\n"
                               "            return(true);\n"
                               "        } else if(*a != *b) {\n"
                               "            return(false);\n"
                               "        }\n"
                               "    }\n"
                               "}\n");



        // TODO(Jonny): typedefs don't work, so I'll need to re-create them too.

        // Forward declare structs.
        {
            write_to_output_buffer(&ob,
                                   "\n"
                                   "//\n"
                                   "// Forward declared structs, enums, and functions\n"
                                   "//\n");

            for(Int i = 0; (i < pr.structs.cnt); ++i) {
                StructData *sd = pr.structs.e + i;

                //if(sd->template_header.len) {
                //    write_to_output_buffer(&ob, "template%.*s", sd->template_header.len, sd->template_header.e);
                //}

                if(sd->struct_type == StructType_struct) {
                    write_to_output_buffer(&ob, "struct %.*s;\n", sd->name.len, sd->name.e);
                } else if(sd->struct_type == StructType_class) {
                    write_to_output_buffer(&ob, "class %.*s;\n", sd->name.len, sd->name.e);
                } else if(sd->struct_type == StructType_union) {
                    write_to_output_buffer(&ob, "union %.*s;\n", sd->name.len, sd->name.e);
                } else {
                    assert(0);
                }
            }

            // Forward declared enums.
            for(Int i = 0; (i < pr.enums.cnt); ++i) {
                EnumData *ed = pr.enums.e + i;

                if(ed->type.len) {
                    write_to_output_buffer(&ob,
                                           "enum %s%.*s : %.*s;\n",
                                           (ed->is_struct) ? "class " : "",
                                           ed->name.len, ed->name.e,
                                           ed->type.len, ed->type.e);

                }
            }

            // Forward declared functions.
            for(Int i = 0; (i < pr.funcs.cnt); ++i) {
                FunctionData *fd = pr.funcs.e + i;

                Char result_ptr_buf[max_ptr_size] = {};
                for(Int j = 0; (j < fd->return_type_ptr); ++j) {
                    result_ptr_buf[j] = '*';
                }

                write_to_output_buffer(&ob, "%.*s %.*s%s %.*s(",
                                       fd->linkage.len, fd->linkage.e,
                                       fd->return_type.len, fd->return_type.e,
                                       result_ptr_buf,
                                       fd->name.len, fd->name.e);

                for(Int j = 0; (j < fd->param_cnt); ++j) {
                    Variable *v = fd->params + j;

                    Char ptr_buf[max_ptr_size] = {};
                    for(Int k = 0; (k < v->ptr); ++k) {
                        ptr_buf[k] = '*';
                    }

                    write_to_output_buffer(&ob, "%.*s %s %.*s",
                                           v->type.len, v->type.e,
                                           ptr_buf,
                                           v->name.len, v->name.e);

                    if(j < fd->param_cnt - 1) {
                        write_to_output_buffer(&ob, ", ");
                    }
                }

                write_to_output_buffer(&ob, ");\n");
            }
        }

        //
        // Types enum.
        //

        // Get the absolute max number of meta types. This will be significantly bigger than the
        // actual number of unique types...
        Int max_type_count = get_num_of_primitive_types();
        for(Int i = 0; (i < pr.structs.cnt); ++i) {
            max_type_count += pr.structs.e[i].member_count + 1;
        }

        String *types = system_alloc(String, max_type_count);
        if(types) {
            Int type_count = get_actual_type_count(types, pr.structs);
            assert(type_count <= max_type_count);

            write_to_output_buffer(&ob, "\n\n");

            // Recreated enums.
            {
                // TODO(Jonny): I don't think these are actually nessessary for anything. I'm leaving it in just in case
                //              that changes though.
#if 0
                write_to_output_buffer(&ob, "// Recreated structs/enums.\n");

                for(Int i = 0; (i < enum_count); ++i) {
                    EnumData *ed = enum_data + i;

                    write_to_output_buffer(&ob, "enum %s pp_%.*s",
                                           (ed->is_struct) ? "class" : "",
                                           ed->name.len, ed->name.e);
                    if(ed->type.len) {
                        write_to_output_buffer(&ob, " : %.*s",
                                               ed->type.len, ed->type.e);
                    }
                    write_to_output_buffer(&ob, " { ");

                    for(Int j = 0; (j < ed->no_of_values); ++j) {
                        EnumValue *v = ed->values + j;

                        write_to_output_buffer(&ob, "%.*s = %d, ",
                                               v->name.len, v->name.e,
                                               v->value);
                    }

                    write_to_output_buffer(&ob, " };\n");
                }
#endif
            }

            // Recreated structs.
            {
                write_to_output_buffer(&ob,
                                       "//\n"
                                       "// Recreated structs.\n"
                                       "//\n",
                                       "namespace recreated {");
                for(Int i = 0; (i < pr.structs.cnt); ++i) {
                    StructData *sd = pr.structs.e + i;

                    /*if(sd->template_header.len) {
                        //write_to_output_buffer(&ob, "template%.*s ", sd->template_header.len, sd->template_header.e);
                        TempMemory tm = push_temp_memory(sd->template_header.len * 2);
                        Char *buf = cast(Char *)tm.e;
                        Int iter = 0;
                        Char *typename_str = "typename";
                        Int typename_len =  string_length(typename_str);

                        for(Int i = 0; (i < sd->template_header.len); ++i, ++iter) {
                            if(string_compare(sd->template_header.e + i, typename_str, typename_len)) {
                                i += typename_len;
                                for(Int j = 0; (j < typename_len); ++j) {
                                    buf[iter++] = typename_str[j];
                                }

                                while(is_whitespace(sd->template_header.e[i])) {
                                    buf[iter++] = sd->template_header.e[i++];
                                }

                                buf[iter++] = 'p';
                                buf[iter++] = 'p';
                                buf[iter++] = '_';
                                --iter;
                                --i;

                            } else {
                                buf[iter] = sd->template_header.e[i];
                            }
                        }

                        write_to_output_buffer(&ob, "template%.*s", iter, buf);

                        pop_temp_memory(&tm);
                    }*/

                    write_to_output_buffer(&ob, "%s pp_%.*s", (sd->struct_type != StructType_union) ? "struct" : "union",
                                           sd->name.len, sd->name.e);
                    if(sd->inherited) {
                        write_to_output_buffer(&ob, " :");

                        for(Int j = 0; (j < sd->inherited_count); ++j) {
                            String *inherited = sd->inherited + j;

                            if(j > 0) {
                                write_to_output_buffer(&ob, ",");
                            }

                            write_to_output_buffer(&ob, " public pp_%.*s", inherited->len, inherited->e);
                        }
                    }
                    write_to_output_buffer(&ob, " { ");

                    Bool is_inside_anonymous_struct = false;
                    for(Int j = 0; (j < sd->member_count); ++j) {
                        Variable *md = sd->members + j;

                        if(md->is_inside_anonymous_struct != is_inside_anonymous_struct) {
                            is_inside_anonymous_struct = !is_inside_anonymous_struct;

                            if(is_inside_anonymous_struct) {
                                write_to_output_buffer(&ob, " struct {");
                            } else {
                                write_to_output_buffer(&ob, "};");
                            }
                        }

                        Char *arr = "";
                        Char arr_buffer[256] = {};
                        if(md->array_count > 1) {
                            stbsp_snprintf(arr_buffer, 256, "[%u]", md->array_count);
                            arr = arr_buffer;
                        }

                        char ptr_buf[max_ptr_size] = {};
                        for(Int k = 0; (k < md->ptr); ++k) ptr_buf[k] = '*';

                        write_to_output_buffer(&ob, " pp_%.*s %s%.*s%s; ",
                                               md->type.len, md->type.e,
                                               ptr_buf,
                                               md->name.len, md->name.e,
                                               (md->array_count > 1) ? arr_buffer : arr);

                    }

                    if(is_inside_anonymous_struct) write_to_output_buffer(&ob, " };");

                    write_to_output_buffer(&ob, " };\n");
                }
            }

            // Type info definition.
            write_to_output_buffer(&ob,
                                   "\n"
                                   "//\n"
                                   "// Type info definition.\n"
                                   "//\n"
                                   "template<typename T> struct pp_TypeInfo {\n"
                                   "    using type      = void;\n"
                                   "    using weak_type = void;\n"
                                   "    using base      = void;\n"
                                   "\n"
                                   "    static char const * const name;\n"
                                   "    static char const * const weak_name;\n"
                                   "\n"
                                   "    static size_t const member_count;\n"
                                   "    static size_t const base_count;\n"
                                   "\n"
                                   "    static bool const ptr_level;\n"
                                   "    static bool const is_ref;\n"
                                   "\n"
                                   "    static bool const is_primitive;\n"
                                   "    static bool const is_class;\n"
                                   "    static bool const is_enum;\n"
                                   "};\n");

            write_out_type_specification_struct(&ob, pr.structs, pr.enums);

            // pp::TypeInfo for enums.
            {
                for(Int i = 0; (i < pr.enums.cnt); ++i) {
                    EnumData *ed = pr.enums.e + i;

                    write_type_info(&ob, DataType_enum, ed->name, ed->type, ed->no_of_values);
                }
            }

            // Get member at index.
            {
                write_to_output_buffer(&ob,
                                       "// Get at index.\n"
                                       "#define pp_get_member(variable, index) pp_GetMember<decltype(variable), index>::get(variable)\n"
                                       "template<typename T, int index> struct pp_GetMember { };\n");

                for(Int i = 0; (i < pr.structs.cnt); ++i) {
                    StructData *sd = pr.structs.e + i;
                    write_to_output_buffer(&ob, "\n");

                    for(Int j = 0; (j < sd->member_count); ++j) {
                        Variable *md = sd->members + j;

                        Char ptr_buf[max_ptr_size] = {};
                        for(Int k = 0; (k < md->ptr); ++k) {
                            ptr_buf[k] = '*';
                        }

                        // Because get_member _requires_ a pointer, only generate code for the pointer version.
                        write_to_output_buffer(&ob,
                                               "template<> struct pp_GetMember<%.*s *, %d> {\n"
                                               "    static %.*s%s *get(%.*s *ptr) {\n"
                                               "        pp_%.*s *cpy = (pp_%.*s *)ptr;\n"
                                               "        %.*s%s * res = (%.*s%s *)&cpy->%.*s;\n"
                                               "        return(res);\n"
                                               "    };\n"
                                               "};\n",
                                               sd->name.len, sd->name.e,
                                               j,
                                               md->type.len, md->type.e, ptr_buf,
                                               sd->name.len, sd->name.e,
                                               sd->name.len, sd->name.e,
                                               sd->name.len, sd->name.e,
                                               md->type.len, md->type.e, ptr_buf,
                                               md->type.len, md->type.e, ptr_buf,
                                               md->name.len, md->name.e);
                    }
                }
            }

            // Get member name at index.
            {
                write_to_output_buffer(&ob,
                                       "template<typename T>static char const * pp_get_member_name(int index){return(0);}\n");
                for(Int i = 0; (i < pr.structs.cnt); ++i) {
                    StructData *sd = pr.structs.e + i;

                    if(sd->member_count) {
                        write_to_output_buffer(&ob,
                                               "template<>char const * pp_get_member_name<%.*s>(int index){\n"
                                               "    switch(index) {\n",
                                               sd->name.len, sd->name.e);

                        for(Int j = 0; (j < sd->member_count); ++j) {
                            Variable *md = sd->members + j;

                            write_to_output_buffer(&ob,
                                                   "        case %d: { return(\"%.*s\"); } break;\n",
                                                   j,
                                                   md->name.len, md->name.e);
                        }

                        write_to_output_buffer(&ob,
                                               "    }\n"
                                               "    return(0); // Not found.\n"
                                               "}\n");
                    }
                }
            }

            // Get base at index.
            {
                write_to_output_buffer(&ob,
                                       "// Get at index.\n"
                                       "#define pp_get_base(variable, index) pp_GetBase<decltype(variable), index>::get(variable)\n"
                                       "template<typename T, int index> struct pp_GetBase {};\n");

                for(Int i = 0; (i < pr.structs.cnt); ++i) {
                    StructData *sd = pr.structs.e + i;

                    if(sd->inherited_count) {
                        for(Int j = 0; (j < sd->inherited_count); ++j) {
                            StructData *base = find_struct(sd->inherited[j], pr.structs);
                            assert(base); // TODO(Jonny): Better error handling!

                            write_to_output_buffer(&ob,
                                                   "template<> struct pp_GetBase<%.*s *, %d> {\n"
                                                   "    static %.*s *get(%.*s *ptr) {\n"
                                                   "        pp_%.*s *cpy = (pp_%.*s *)ptr;\n"
                                                   "        pp_%.*s *b = dynamic_cast<pp_%.*s *>(cpy);\n"
                                                   "\n"
                                                   "        return((%.*s *)b);\n"
                                                   "    }\n"
                                                   "};\n"
                                                   "\n",
                                                   sd->name.len, sd->name.e, j,
                                                   base->name.len, base->name.e, sd->name.len, sd->name.e,
                                                   sd->name.len, sd->name.e, sd->name.len, sd->name.e,
                                                   base->name.len, base->name.e, base->name.len, base->name.e,
                                                   base->name.len, base->name.e);
                        }
                    }
                }
            }

            // Get access at index.
            {
                write_to_output_buffer(&ob,
                                       "//\n"
                                       "// Get access at index.\n"
                                       "//\n"
                                       "enum pp_Access : int {\n"
                                       "    pp_Access_unknown,\n"
                                       "    pp_Access_public,\n"
                                       "    pp_Access_private,\n"
                                       "    pp_Access_protected,\n"
                                       "\n"
                                       "    pp_Access_count,\n"
                                       "};\n"
                                       "template<typename T, int index> pp_Access pp_get_access_at_index() { return(pp_Access_unknown); }\n");

                auto write_func = [](OutputBuffer *ob, StructData *sd, Int j, char *access, Char *modifier) {
                    write_to_output_buffer(ob,
                                           "template<> pp_Access pp_get_access_at_index<%.*s%s, %d>() { return(pp_Access_%s); }\n",
                                           sd->name.len, sd->name.e, modifier,
                                           j,
                                           access);
                };

                for(Int i = 0; (i < pr.structs.cnt); ++i) {
                    StructData *sd = pr.structs.e + i;

                    write_to_output_buffer(&ob,
                                           "\n"
                                           "//\n"
                                           "// %.*s\n"
                                           "//\n",
                                           sd->name.len, sd->name.e);

                    for(Int j = 0; (j < sd->member_count); ++j) {
                        Variable *md = sd->members + j;

                        Char *access = 0;
                        if(md->access == Access_public)         { access = "public";    }
                        else if(md->access == Access_private)   { access = "private";   }
                        else if(md->access == Access_protected) { access = "protected"; }
                        else                                    { assert(0);            }

                        write_to_output_buffer(&ob,
                                               "\n"
                                               "// %.*s::%.*s\n",
                                               sd->name.len, sd->name.e,
                                               md->name.len, md->name.e);
                        write_func(&ob, sd, j, access, "");
                        write_func(&ob, sd, j, access, " *");
                        write_func(&ob, sd, j, access, " **");
                        write_func(&ob, sd, j, access, " ***");
                        write_func(&ob, sd, j, access, " &");
                        write_func(&ob, sd, j, access, " *&");
                        write_func(&ob, sd, j, access, " **&");
                        write_func(&ob, sd, j, access, " ***&");
                    }
                }
            }


            system_free(types);
        }

        write_to_output_buffer(&ob,
                               "\n"
                               "//\n"
                               "// Enum Introspection data.\n"
                               "//\n"
                               "\n"
                               "// Stub functions.\n"
                               "template<typename T>static char const *pp_enum_to_string(T element) { return(0); }\n"
                               "template<typename T>static T pp_string_to_enum(char const *str) { return(0); }\n"
                               "\n");
        for(Int i = 0; (i < pr.enums.cnt); ++i) {
            EnumData *ed = pr.enums.e + i;

            if(ed->type.len) {
                write_to_output_buffer(&ob,
                                       "// %.*s.\n",
                                       ed->name.len, ed->name.e);
                // Enum to string.
                {
                    write_to_output_buffer(&ob,
                                           "template<>char const *pp_enum_to_string<%.*s>(%.*s element) {\n"
                                           "    %.*s index = (%.*s)element;\n"
                                           "    switch(index) {\n",
                                           ed->name.len, ed->name.e,
                                           ed->name.len, ed->name.e,
                                           ed->type.len, ed->type.e,
                                           ed->type.len, ed->type.e);
                    for(Int j = 0; (j < ed->no_of_values); ++j) {
                        EnumValue *v = ed->values + j;

                        write_to_output_buffer(&ob,
                                               "        case %d:  { return(\"%.*s\"); } break;\n",
                                               v->value,
                                               v->name.len, v->name.e);
                    }

                    write_to_output_buffer(&ob,
                                           "\n"
                                           "        default: { return(0); } break;\n"
                                           "    }\n"
                                           "}\n");
                }

                // String to enum.
                {
                    write_to_output_buffer(&ob,
                                           "template<>%.*s pp_string_to_enum<%.*s>(char const *str) {\n"
                                           "    %.*s res = 0;\n"
                                           "\n",
                                           ed->name.len, ed->name.e,
                                           ed->name.len, ed->name.e,
                                           ed->type.len, ed->type.e);
                    for(Int j = 0; (j < ed->no_of_values); ++j) {
                        EnumValue *v = ed->values + j;

                        if(!j) {
                            write_to_output_buffer(&ob,
                                                   "    ");
                        } else {
                            write_to_output_buffer(&ob,
                                                   "    else ");

                        }

                        write_to_output_buffer(&ob,
                                               "if(!pp_string_compare(\"%.*s\", str)) {\n"
                                               "        res = (%.*s)%d;\n"
                                               "    }\n",
                                               v->name.len, v->name.e,
                                               ed->name.len, ed->name.e,
                                               v->value);
                    }

                    write_to_output_buffer(&ob,
                                           "\n"
                                           "    return (%.*s)res; \n"
                                           "}\n",
                                           ed->name.len, ed->name.e);
                }
            }
        }

        // TODO(Jonny): A lot of the function stuff won't actually work if you pass a struct into a function. This may
        //              be fixable if I generate a .hpp file and a .cpp file.

        //
        // Functors
        //
        {
#if 0
            write_to_output_buffer(&ob,
                                   "\n"
                                   "//\n"
                                   "// Function stuff.\n"
                                   "//\n"
                                   "#define pp_get_functor(func) pp_Functor_##func\n"
                                   "\n"
                                   "enum pp_Linkage : int {\n"
                                   "    pp_Linkage_default,\n"
                                   "    pp_Linkage_static,\n"
                                   "    pp_Linkage_inline,\n"
                                   "};\n"
                                   "\n"
                                   "struct pp_FunctionData {\n"
                                   "    char const *return_type;\n"
                                   "    pp_Linkage linkage;\n"
                                   "};\n");

            for(Int i = 0; (i < func_count); ++i) {
                FunctionData *fd = func_data + i;

                PtrSize size = scratch_memory_size / 4;
                Char *buf1 = cast(Char *)push_scratch_memory(size); Int wrtn1 = 0; // Param list.
                Char *buf2 = cast(Char *)push_scratch_memory(size); Int wrtn2 = 0; // Calling func from param list.
                Char *buf3 = cast(Char *)push_scratch_memory(size); Int wrtn3 = 0; // Calling func from param list with this.
                Char *buf4 = cast(Char *)push_scratch_memory(size); Int wrtn4 = 0; // Params as member variables

                for(Int j = 0; (j < fd->param_cnt); ++j) {
                    Variable *param = fd->params + j;

                    Char *write_comma = (j < fd->param_cnt - 1) ? ", " : "";

                    Char ptr_buf[max_ptr_size] = {};
                    for(Int k = 0; (k < param->ptr); ++k) {
                        ptr_buf[k] = '*';
                    }

                    // buf1
                    wrtn1 += stbsp_snprintf(buf1 + wrtn1, size - wrtn1,
                                            "%.*s %s_%.*s%s",
                                            param->type.len, param->type.e,
                                            ptr_buf,
                                            param->name.len, param->name.e,
                                            write_comma);

                    // buf2
                    wrtn2 += stbsp_snprintf(buf2 + wrtn2, size - wrtn2,
                                            "_%.*s%s",
                                            param->name.len, param->name.e,
                                            write_comma);

                    // buf3
                    wrtn3 += stbsp_snprintf(buf3 + wrtn3, size - wrtn3,
                                            "this->%.*s%s",
                                            param->name.len, param->name.e,
                                            write_comma);

                    // buf4
                    wrtn4 += stbsp_snprintf(buf4 + wrtn4, size - wrtn4,
                                            "    %.*s %s%.*s;\n",
                                            param->type.len, param->type.e,
                                            ptr_buf,
                                            param->name.len, param->name.e);
                }


                write_to_output_buffer(&ob,
                                       "// %.*s\n"
                                       "struct pp_Functor_%.*s {\n"
                                       "    %.*s operator()(%s) { return %.*s(%s); }\n",
                                       fd->name.len, fd->name.e,
                                       fd->name.len, fd->name.e,
                                       fd->return_type.len, fd->return_type.e, buf1, fd->name.len, fd->name.e, buf2);

                if(fd->param_cnt) {
                    write_to_output_buffer(&ob,
                                           "    %.*s operator()() { return %.*s(%s); }\n"
                                           "\n"
                                           "%s",
                                           fd->return_type.len, fd->return_type.e, fd->name.len, fd->name.e, buf3,
                                           buf4);
                }
                write_to_output_buffer(&ob,
                                       "};\n"
                                       "\n");

                clear_scratch_memory();
            }
#endif
        }

        //
        // # Guard macro.
        //
        write_to_output_buffer(&ob,
                               "\n"
                               "#undef pp_std\n"
                               "\n"
                               "#define PP_GENERATED_H\n"
                               "#endif // #if defined(PP_GENERATED_H)\n"
                               "\n");

        res.size = ob.index;
        res.data = ob.buffer;
    }

    return(res);
}
