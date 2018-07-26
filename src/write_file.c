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

typedef struct {
    Char *buffer;
    Int index;
    Int size;
} OutputBuffer;

Void write_ob(OutputBuffer *ob, Char *format, ...) {
    va_list args;
    va_start(args, format);
    ob->index += stbsp_vsnprintf(ob->buffer + ob->index, ob->size - ob->index, format, args);
    assert(ob->index < ob->size);
    va_end(args);
}

Void write_to_output_buffer_no_var_args(OutputBuffer *ob, Char *format) {
    Char *fat = format;
    Char *obat = ob->buffer + ob->index;
    while(*fat) {
        ++ob->index;
        *obat++ = *fat++;
    }
}

global Char *global_primitive_types[] = {
    "char", "short", "int", "long", "float", "double", //"bool",
    "uint64_t", "uint32_t", "uint16_t", "uint8_t",
    "int64_t", "int32_t", "int16_t", "int8_t",
    "uintptr_t", "intptr_t", "size_t"
};

Bool is_primitive(String str) {
    Bool res = false;
    for(Int i = 0; (i < array_count(global_primitive_types)); ++i) {
        if(string_cstring_comp(str, global_primitive_types[i])) {
            res = true;
            break;
        }
    }

    return(res);
}

// TODO - Hacky as fuck.
Bool is_pp_type(String str) {
    Bool res = false;
    if(str.len > 3) {
        if(str.e[0] == 'p' && str.e[1] == 'p' && str.e[2] == '_') {
            res = true;
        }
    }

    return(res);
}

Struct_Data *find_struct(String str, Structs structs) {
    Struct_Data *res = 0;

    if(str.len) {
        for(Int i = 0; (i < structs.cnt); ++i) {
            Struct_Data *sd = structs.e + i;

            if(string_comp(str, sd->name)) {
                res = sd;
                break; // for
            }
        }
    }

    return(res);
}

Bool is_meta_type_already_in_array(String *array, Int len, String test) {
    Bool res = false;

    for(Int i = 0; (i < len); ++i) {
        if(string_comp(array[i], test)) {
            res = true;
            break;
        }
    }

    return(res);
}

Int get_actual_type_count(String *types, Structs structs, Enums enums, Typedefs typedefs) {
    Int res = 0;

    // Primitives.
    for(Int i = 0; (i < array_count(global_primitive_types)); ++i) {
        types[res++] = create_string(global_primitive_types[i]);
    }

    // Typedefs.
    for(Int i = 0; (i < typedefs.cnt); ++i) {
        Typedef_Data *td = typedefs.e + i;

        if(!is_meta_type_already_in_array(types, res, td->fresh)) {
            types[res++] = td->fresh;
        }
    }

    // Enums
    for(Int i = 0; (i < enums.cnt); ++i) {
        Enum_Data *ed = enums.e + i;

        if(!is_meta_type_already_in_array(types, res, ed->name)) {
            types[res++] = ed->name;
        }
    }

    // Fill out the enum meta type enum.
    for(Int i = 0; (i < structs.cnt); ++i) {
        Struct_Data *sd = structs.e + i;

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

String get_original_type_for_typedef(Typedef_Data td, Typedefs typedefs) {
    String result = td.original;

    for(Int i = 0; (i < typedefs.cnt); ++i) {
        if(string_comp(typedefs.e[i].fresh, result)) {
            result = typedefs.e[i].original;
            i = 0;
        }
    }

    return(result);
}

Bool is_typedef_for_void(String str, Typedefs typedefs) {
    for(Int i = 0; (i < typedefs.cnt); ++i) {
        String original = get_original_type_for_typedef(typedefs.e[i], typedefs);
        if(string_comp(typedefs.e[i].fresh, str) && string_cstring_comp(original, "void")) {
            return(true);
        }
    }

    return(false);
}

Bool is_unsized_enum(String str, Enums enums) {
    for(Int i = 0; (i < enums.cnt); ++i) {
        if(string_comp(str, enums.e[i].name)) {
            if(!enums.e[i].type.len) {
                return(true);
            }
        }
    }

    return(false);
}

Void write_get_size_from_type(OutputBuffer *ob, String *types, Int type_count, Typedefs typedefs, Enums enums) {
    write_ob(ob,
             "\n"
             "PP_STATIC uintptr_t pp_get_size_from_type(pp_Type type) {\n"
             "    switch(pp_typedef_to_original(type)) {\n");
    for(Int i = 0; (i < type_count); ++i) {
        // TODO - is_typedef_for_void isn't working.
        if(!string_cstring_comp(types[i], "void") && !is_typedef_for_void(types[i], typedefs)) {
            String cpy = (is_unsized_enum(types[i], enums)) ? create_string("int") : types[i];

            if(!is_pp_type(types[i])) {
                write_ob(ob,
                         "        case pp_Type_%.*s: { return sizeof(pp_%.*s); } break;\n",
                         types[i].len, types[i].e, cpy.len, cpy.e);
            }
        }
    }

    write_ob(ob,
             "    }\n"
             "\n"
             "    PP_ASSERT(0);\n"
             "    return(0);\n"
             "}\n");
}

void write_enum_size_data(OutputBuffer *ob, Enums enums) {
    write_ob(ob,
             "\n"
             "//\n"
             "// Number of members in an enum.\n"
             "//\n");
#if 1
    // Constant version.
    {
        write_ob(ob, "#define pp_get_enum_size_const(type) pp_get_enum_size_##type\n");

        for(Int i = 0; (i < enums.cnt); ++i) {
            Enum_Data *ed = enums.e + i;

            write_ob(ob,
                     "#define pp_get_enum_size_%.*s %d\n",
                     ed->name.len, ed->name.e,
                     ed->no_of_values);
        }

    }
#endif

    {
        write_ob(ob,
                 "\n"
                 "//\n"
                 "// Number of members in an enum.\n"
                 "//\n"
                 "PP_STATIC uintptr_t pp_get_enum_size_from_type(pp_Type type) {\n");

        if(enums.cnt) {
            write_ob(ob,
                     "    switch(pp_typedef_to_original(type)) {\n");

            for(Int i = 0; (i < enums.cnt); ++i) {
                Enum_Data *ed = enums.e + i;

                write_ob(ob,
                         "        case pp_Type_%.*s: { return(%d); } break;\n",
                         ed->name.len, ed->name.e,
                         ed->no_of_values);
            }

            write_ob(ob,
                     "    }\n");
        }

        write_ob(ob,
                 "\n"
                 "    PP_ASSERT(0);\n"
                 "    return(0);\n"
                 "}\n");
    }
}

typedef struct {
    String *types;
    Int count;
} TypeInformation;
TypeInformation create_type_information(Parse_Result pr) {
    TypeInformation res = {0};

    Int max_type_count = array_count(global_primitive_types) + pr.structs.cnt + pr.enums.cnt + pr.typedefs.cnt;
    for(Int i = 0; (i < pr.structs.cnt); ++i) {
        max_type_count += pr.structs.e[i].member_count;
    }

    res.types = system_malloc_arr(sizeof(*res.types), max_type_count + 2); // Plus 2 because I manually add __m128 and __m128i

    if(res.types) {
        res.count = get_actual_type_count(res.types, pr.structs, pr.enums, pr.typedefs);

        // Add __m128 and __m128i if they weren't already added.
        if(!is_meta_type_already_in_array(res.types, res.count, create_string("__m128"))) {
            res.types[res.count++] = create_string("__m128");
        }
        if(!is_meta_type_already_in_array(res.types, res.count, create_string("__m128i"))) {
            res.types[res.count++] = create_string("__m128i");
        }

#if INTERNAL
        for(Int i = 0; (i < res.count); ++i) {
            assert(is_valid_iden_name(res.types[i]));
        }
#endif
    }

    return(res);
}

Void write_pp_type_enum(OutputBuffer *ob, TypeInformation ti) {
    write_ob(ob,
             "\n"
             "//\n"
             "// An enum, with an index for each type in the codebase.\n"
             "//\n"
             "typedef enum pp_Type {\n"
             "    pp_Type_unknown,\n");

    for(Int i = 0; (i < ti.count); ++i) {
        write_ob(ob,
                 "    pp_Type_%.*s,\n",
                 ti.types[i].len, ti.types[i].e);
    }

    if(!is_meta_type_already_in_array(ti.types, ti.count, create_string("void"))) {
        write_ob(ob, "   pp_Type_void,");
    }

    write_ob(ob,
             "\n"
             "    pp_Type_count\n"
             "} pp_Type;\n");
}

Void forward_declare_enums(OutputBuffer *ob, Enums enums) {
    write_ob(ob,
             "\n"
             "/* Forward declared enums. */\n"
             "#if !defined(__cplusplus)\n");

    for(Int i = 0; (i < enums.cnt); ++i) {
        Enum_Data *ed = enums.e + i;

        if(ed->type.len) {
            write_ob(ob, "    enum %.*s : %.*s;\n", ed->name.len, ed->name.e, ed->type.len, ed->type.e);
        } else {
            write_ob(ob, "    typedef enum %.*s %.*s;\n", ed->name.len, ed->name.e, ed->name.len, ed->name.e);
        }
    }

    write_ob(ob, "#endif // defined(__cplusplus)\n");

}

Void forward_declare_structs(OutputBuffer *ob, Structs structs) {
    write_ob(ob,
             "\n"
             "/* Forward declared structs. */\n");
    for(Int i = 0; (i < structs.cnt); ++i) {
        Struct_Data *sd = structs.e + i;

        switch(sd->struct_type) {
            case StructType_struct: write_ob(ob, "typedef struct "); break;
            case StructType_class:  write_ob(ob, "class ");          break;
            case StructType_union:  write_ob(ob, "typedef union ");  break;

            default: assert(0); break;
        }

        write_ob(ob, "%.*s %.*s;\n", sd->name.len, sd->name.e, sd->name.len, sd->name.e);
    }
}

Void recreate_typedefs(OutputBuffer *ob, Typedefs typedefs) {
    write_ob(ob,
             "\n"
             "//\n"
             "// Create typedefs.\n"
             "//\n");

    for(Int i = 0; (i < typedefs.cnt); ++i) {
        Typedef_Data *td = typedefs.e + i;

        write_ob(ob,
                 "typedef pp_%.*s pp_%.*s;\n",
                 td->original.len, td->original.e,
                 td->fresh.len, td->fresh.e);
    }
}

Void recreate_structs(OutputBuffer *ob, Structs structs) {
    write_ob(ob,
             "\n"
             "//\n"
             "// Recreated structs.\n"
             "//\n");
    for(Int i = 0; (i < structs.cnt); ++i) {
        Struct_Data *sd = structs.e + i;

        write_ob(ob, "%s pp_%.*s", (sd->struct_type != StructType_union) ? "struct" : "union",
                 sd->name.len, sd->name.e);

        write_ob(ob, " {\n    ");

        /* This won't work for
            struct Foo {
                struct { union {}; }
            }

            but should work for just one level.
        */
        AnonymousStruct anonymous_struct = AnonymousStruct_none;
        for(Int j = 0; (j < sd->member_count); ++j) {
            Variable *md = sd->members + j;

            if(md->anonymous_struct != anonymous_struct) {
                if(anonymous_struct) {
                    anonymous_struct = AnonymousStruct_none;
                } else {
                    anonymous_struct = md->anonymous_struct;
                }

                if(anonymous_struct) {
                    Char *s = anonymous_struct_to_type(anonymous_struct);
                    write_ob(ob, " %s {", s);
                } else {
                    write_ob(ob, "};");
                }
            }

            // Array.
            Char *arr = "";
            Char arr_buffer[256] = {0};
            if(md->array_count > 1) {
                stbsp_snprintf(arr_buffer, 256, "[%u]", md->array_count);
                arr = arr_buffer;
            }

            // Pointer.
            char ptr_buf[MAX_POINTER_SIZE] = {0};
            for(Int k = 0; (k < md->ptr); ++k) {
                ptr_buf[k] = '*';
            }

            // Modifiers
            Char modifier_string[1024] = {0};
            if(md->modifier) {
                Uintptr mod_string_index = 0;

                if(md->modifier & Modifier_unsigned) {
                    mod_string_index += string_copy(modifier_string + mod_string_index, " unsigned");
                }

                if(md->modifier & Modifier_const) {
                    mod_string_index += string_copy(modifier_string + mod_string_index, " const");
                }

                if(md->modifier & Modifier_volatile) {
                    mod_string_index += string_copy(modifier_string + mod_string_index, " volatile");
                }

                if(md->modifier & Modifier_mutable) {
                    mod_string_index += string_copy(modifier_string + mod_string_index, " mutable");

                }
                if(md->modifier & Modifier_signed) {
                    mod_string_index += string_copy(modifier_string + mod_string_index, " signed");
                }
            }

            // If the type is not a primitive or pp_type, prepend "pp_" onto the front of it.
            if(!is_primitive(md->type) && !is_pp_type(md->type)) {
                write_ob(ob, "pp_");
            }

            // Write out all data.
            write_ob(ob, "%.*s%s %s%.*s%s; ",
                     md->type.len, md->type.e,
                     modifier_string,
                     ptr_buf,
                     md->name.len, md->name.e,
                     (md->array_count > 1) ? arr_buffer : arr);

        }

        if(anonymous_struct) {
            write_ob(ob, " };");
        }

        write_ob(ob, "\n};\n");

    }
}

Void typedef_to_original_function(OutputBuffer *ob, Typedefs typedefs) {
    write_ob(ob,
             "\n"
             "// Turn a typedef'd type into it's original type.\n"
             "PP_STATIC pp_Type pp_typedef_to_original(pp_Type type) {\n");
    if(typedefs.cnt) {
        write_ob(ob, "    switch(type) {\n");

        for(Int i = 0; (i < typedefs.cnt); ++i) {
            String original = get_original_type_for_typedef(typedefs.e[i], typedefs);

            write_ob(ob,
                     "        case pp_Type_%.*s: { return(pp_Type_%.*s); } break;\n",
                     typedefs.e[i].fresh.len, typedefs.e[i].fresh.e,
                     original.len, original.e);
        }

        write_ob(ob,
                 "    }\n");
    }

    write_ob(ob,
             "\n"
             "    return(type);\n"
             "}\n");
}

Void forward_declare_functions(OutputBuffer *ob, Functions functions) {
    write_ob(ob,
             "\n"
             "/* Forward declared functions. */\n");
    for(Int i = 0; (i < functions.cnt); ++i) {
        Function_Data *fd = functions.e + i;

        Char ptr_buf[MAX_POINTER_SIZE] = {0};
        for(Int j = 0; (j < fd->return_type_ptr); ++j) {
            ptr_buf[j] = '*';
        }

        write_ob(ob,
                 "%.*s %.*s%s %.*s(",
                 fd->linkage.len, fd->linkage.e,
                 fd->return_type.len, fd->return_type.e,
                 ptr_buf,
                 fd->name.len, fd->name.e);

        for(Int j = 0; (j < fd->param_cnt); ++j) {
            Variable *param = fd->params + j;

            Char param_ptr_buf[MAX_POINTER_SIZE] = {0};
            for(Int k = 0; (k < param->ptr); ++k) {
                param_ptr_buf[k] = '*';
            }

            if(j) {
                write_ob(ob, ", ");
            }

            Char modifier_string[128] = {0};
            if(param->modifier) {
                // Modifiers
                if(param->modifier) {
                    Uintptr mod_string_index = 0;

                    if(param->modifier & Modifier_unsigned) {
                        mod_string_index += string_copy(modifier_string + mod_string_index, " unsigned");
                    }

                    if(param->modifier & Modifier_const) {
                        mod_string_index += string_copy(modifier_string + mod_string_index, " const");
                    }

                    if(param->modifier & Modifier_volatile) {
                        mod_string_index += string_copy(modifier_string + mod_string_index, " volatile");
                    }

                    if(param->modifier & Modifier_mutable) {
                        mod_string_index += string_copy(modifier_string + mod_string_index, " mutable");

                    }
                    if(param->modifier & Modifier_signed) {
                        mod_string_index += string_copy(modifier_string + mod_string_index, " signed");
                    }
                }
            }

            write_ob(ob,
                     "%.*s %s %s %.*s",
                     param->type.len, param->type.e,
                     modifier_string,
                     param_ptr_buf,
                     param->name.len, param->name.e);
        }

        write_ob(ob, ");\n");
    }
}

File write_data(Parse_Result pr) {
    File res = {0};
    OutputBuffer ob = {0};
    ob.size = 1024 * 1024;
    //ob.buffer = new Char[ob.size];
    ob.buffer = system_malloc_arr(sizeof * ob.buffer, ob.size);

    TypeInformation type_information = create_type_information(pr);
    if(ob.buffer) {
        write_ob(&ob,
                 "#if !defined(PP_GENERATED_H)\n"
                 "\n"
                 "#if defined(__cplusplus)\n"
                 "extern \"C\" {\n"
                 "#endif\n"
                 "\n"
                 "#define PP_IGNORE\n"
                 "#define PP_TEST\n"
                 "\n"
                 "#include <stdint.h>\n"
                 "#include <string.h>\n"
                 "\n"
                 "#if !defined(PP_ASSERT)\n"
                 "    #include <assert.h>\n"
                 "    #define PP_ASSERT assert\n"
                 "#endif\n"
                 "\n"
                 "#if !defined(PP_SPRINTF)\n"
                 "    #include \"stdio.h\" \n"
                 "    #if defined(_MSC_VER)\n"
                 "        #define PP_SPRINTF(buf, size, format, ...) sprintf_s(buf, size, format, ##__VA_ARGS__)\n"
                 "    #else\n"
                 "        #define PP_SPRINTF(buf, size, format, ...) sprintf(buf, format, ##__VA_ARGS__)\n"
                 "    #endif\n"
                 "#endif\n"
                 "\n"
                 "#define PP_CONCAT(a, b) a##b\n"
                 "#define PP_TO_STRING(a) #a\n"
                 "\n"
                 "#define PP_OFFSETOF(T, member) ((uintptr_t)&(((T *)0)->member))\n"
                 "\n"
                 "#if defined(PP_STATIC_FUNCS)\n"
                 "    #define PP_STATIC static\n"
                 "#else\n"
                 "    #define PP_STATIC\n"
                 "#endif\n"
                 "\n"
                 "#define PP_TRUE 1\n"
                 "#define PP_FALSE 0\n"
                 "\n"
                 "typedef struct pp___m128 { float e[4]; } pp___m128;\n"
                 "typedef struct pp___m128i { int e[4]; } pp___m128i;\n");

        write_ob(&ob,
                 "\n// Primitive types.\n"
                 "typedef void pp_void;\n"
                 "typedef int32_t pp_MyBool;");


        for(Int i = 0; (i < array_count(global_primitive_types)); ++i) {
            Char *p = global_primitive_types[i];

            write_ob(&ob,
                     "typedef %s pp_%s;\n",
                     p, p);
        }

        write_ob(&ob,
                 "\n"
                 "PP_STATIC pp_MyBool pp_string_compare(char const *a, char const *b) {\n"
                 "    for(; (*a != *b); ++a, ++b) {\n"
                 "        if(!(*a) && !(*b)) {\n"
                 "            return(PP_TRUE);\n"
                 "        }\n"
                 "    }\n"
                 "\n"
                 "    return(PP_TRUE);\n"
                 "}\n"
                 "\n"
                 "#if !defined(PP_MEMSET)\n"
                 "PP_STATIC void *PP_MEMSET(void *dst, uint8_t v, uintptr_t size) {\n"
                 "    uint8_t *dst8 = (uint8_t *)dst;\n"
                 "    while(size-- > 0) {\n"
                 "        *dst8++ = v;\n"
                 "    }\n"
                 "\n"
                 "    return(dst);\n"
                 "}\n"
                 "#endif");

        // Dynamic Array
#if 0
        {
            write_ob(&ob,
                     "#define pp_da_push(arr, element)       (pp_da_check_if_need_to_grow(arr, 1), (arr)[pp_da_get_cnt(arr)++] = (element))\n"
                     "#define pp_da_pop(arr)                 (arr)[--pp_da_get_cnt(arr)];\n"
                     "#define pp_da_size(arr)                ((arr) ? pp_da_get_cnt(arr) : 0)\n"
                     "#define pp_da_free(arr)                ((arr) ? free(pp_da_raw_ptr(arr)), arr = 0);\n"
                     "#define pp_da_get_last(arr)            ((arr) ? (arr)[pp_da_get_cnt(arr) - 1] : 0)\n"
                     "#define pp_da_get_first(arr)           ((arr) ? (arr)[0] : 0)\n"
                     "#define pp_da_is_empty(arr)            (((arr) == 0) || (pp_da_size(arr) == 0))\n"
                     "#define pp_da_expand(arr, num)         (pp_da_check_if_need_to_grow(arr, num), pp_da_get_cnt(arr) += (num), &(arr)[pp_da_get_cnt(arr) - (num)])\n"
                     "#define pp_da_assert_index(arr, Index) do { PP_ASSERT(arr); PP_ASSERT((Index) < pp_da_size(arr)); } while(0) \n"
                     "\n"
                     "#define pp_da_copy_entire(src, dst) do { PP_ASSERT(src); for(uintptr_t ArrayIndex = 0, ArrayOneSize = pp_da_size(src); (ArrayIndex < ArrayOneSize); ++ArrayIndex) { pp_da_assert_index((src), ArrayIndex); pp_da_push(dst, (src)[ArrayIndex]); } } while(0)\n"
                     "\n"
                     "#define pp_da_insert(arr, insert_index, element) do { pp_da_expand(arr, 1); for(uintptr_t ArrayIndex = pp_da_size(arr) - 1; (ArrayIndex > (insert_index)); --ArrayIndex) { pp_da_assert_index(arr, ArrayIndex); (arr)[ArrayIndex] = (arr)[ArrayIndex - 1]; } (arr)[(insert_index)] = (element); } while(0) \n"
                     "\n"
                     "#define pp_da_remove(arr, index_to_remove) do { (arr)[(index_to_remove)]; pp_da_assert_index((arr), (index_to_remove)); pp_da_push((arr), (arr)[index_to_remove]); for(uintptr_t ArrayIndex = (index_to_remove), ArraySizeMinusOne = (pp_da_size(arr) - 1); (ArrayIndex < ArraySizeMinusOne); ++ArrayIndex) { pp_da_assert_index(arr, ArrayIndex); (arr)[ArrayIndex] = (arr)[ArrayIndex + 1]; } --pp_da_get_cnt(arr); } while(0) \n"
                     "\n"
                     "/* These methods shouldn't be directly used. */\n"
                     "#define pp_da_raw_ptr(arr) ((uintptr_t * )(arr) - 2)\n"
                     "#define pp_da_mem_size(arr) pp_da_raw_ptr(arr)[0]\n"
                     "#define pp_da_get_cnt(arr) pp_da_raw_ptr(arr)[1]\n"
                     "#define pp_da_should_grow(arr, number) (((arr) == 0) || (pp_da_get_cnt(arr) + (number) >= pp_da_mem_size(arr)))\n"
                     "#define pp_da_check_if_need_to_grow(arr, number) (pp_da_should_grow(arr,(number)) ? pp_da_grow(arr, number) : 0)\n"
                     "\n"
                     "#define pp_da_grow(arr, number) ((arr) = (decltype(arr))pp_da_grow_((arr), (number), sizeof(*(arr))))\n"
                     "\n"
                     "PP_STATIC void *pp_da_grow_(void *arr, uintptr_t Increment, uintptr_t ItemSize) {\n"
                     "    void *res = 0; \n"
                     "    uintptr_t double_cur_size = (arr) ? 2 * pp_da_mem_size(arr) : 0; \n"
                     "    uintptr_t min_size_needed = pp_da_size(arr) + Increment; \n"
                     "    uintptr_t mem_size = (double_cur_size > min_size_needed) ? double_cur_size : min_size_needed; \n"
                     "    uintptr_t *raw_ptr = (arr) ? pp_da_raw_ptr(arr) : 0; \n"
                     "    uintptr_t *new_raw_ptr = (uintptr_t *)PP_REALLOC(raw_ptr, (ItemSize * mem_size) + (sizeof(uintptr_t) * 2)); \n"
                     "\n"
                     "    if(new_raw_ptr) {\n"
                     "        if(!arr) {\n"
                     "            new_raw_ptr[1] = 0; \n"
                     "        }\n"
                     "\n"
                     "        new_raw_ptr[0] = mem_size; \n"
                     "        res = (new_raw_ptr + 2); \n"
                     "    } else {\n"
                     "        PP_ASSERT(0);\n"
                     "    }\n"
                     "\n"
                     "    return(res); \n"
                     "}\n");
        }
#endif

        write_pp_type_enum(&ob, type_information);

        // Forward declared structs/functions.
        write_ob(&ob,
                 "\n"
                 "//\n"
                 "// Forward declared structs, enums, and functions\n"
                 "//\n"
                 "#if !defined(PP_NO_FORWARD_DECLARE)\n");

        forward_declare_enums(&ob, pr.enums);
        forward_declare_structs(&ob, pr.structs);
        forward_declare_functions(&ob, pr.funcs);

        write_ob(&ob, "#endif // PP_NO_FORWARD_DECLARE\n");

        write_ob(&ob,
                 "\n"
                 "//\n"
                 "// Forward declared recreated stuff.\n"
                 "//\n");

        // Forward declared recreated structs.
        // TODO(Jonny): Can I move these into the other forward declare parts?
        write_ob(&ob, "\n// Forward declared structs.\n");
        for(Int i = 0; (i < pr.structs.cnt); ++i) {
            Struct_Data *sd = pr.structs.e + i;

            Char *type = 0;
            switch(sd->struct_type) {
                case StructType_struct: { type = "struct"; } break;
                case StructType_class:  { type = "class"; } break;
                case StructType_union:  { type = "union"; } break;

                default: { assert(0); } break;
            }

            write_ob(&ob,
                     "typedef %s pp_%.*s pp_%.*s;    "
                     "typedef %s pp_%.*s pp_pp_%.*s;\n",
                     type,
                     sd->name.len, sd->name.e,
                     sd->name.len, sd->name.e,
                     type,
                     sd->name.len, sd->name.e,
                     sd->name.len, sd->name.e);
        }

        // Forward declared recreated enums.
        write_ob(&ob, "\n// Forward declared enums\n");
        for(Int i = 0; (i < pr.enums.cnt); ++i)  {
            Enum_Data *ed = pr.enums.e + i;

            if(ed->type.len) {
                write_ob(&ob,
                         "enum pp_%.*s : %.*s;\n",
                         ed->name.len, ed->name.e,
                         ed->type.len, ed->type.e);
            } else {
                write_ob(&ob,
                         "typedef int pp_%.*s;\n",
                         ed->name.len, ed->name.e);
            }
        }

        recreate_typedefs(&ob, pr.typedefs);
        recreate_structs(&ob, pr.structs);
        typedef_to_original_function(&ob, pr.typedefs);

        // get members from type.
        {
            write_ob(&ob,
                     "typedef struct pp_MemberDefinition {\n"
                     "    pp_Type type;\n"
                     "    char const *name;\n"
                     "    uintptr_t offset;\n"
                     "    uintptr_t ptr;\n"
                     "    uintptr_t arr_size;\n"
                     "} pp_MemberDefinition;\n"
                     "\n"
                     "PP_STATIC pp_MemberDefinition pp_get_members_from_type(pp_Type type, uintptr_t index) {\n"
                     "    pp_MemberDefinition failres;\n"
                     "    pp_Type real_type = pp_typedef_to_original(type);\n"
                     "    if(real_type == pp_Type___m128) {\n"
                     "        switch(index) {\n"
                     "            case 0: {\n"
                     "                pp_MemberDefinition res = {pp_Type_float, \"e\", PP_OFFSETOF(pp___m128, e), 0, 4};\n"
                     "                return(res);\n"
                     "            }\n"
                     "        }\n"
                     "    }\n"
                     "    else if(real_type == pp_Type___m128i) {\n"
                     "        switch(index) {\n"
                     "            case 0: {\n"
                     "                pp_MemberDefinition res = {pp_Type_float, \"e\", PP_OFFSETOF(pp___m128i, e), 0, 4};\n"
                     "                return(res);\n"
                     "            }\n"
                     "        }\n"
                     "    }\n");

            for(Int i = 0; (i < pr.structs.cnt); ++i) {
                Struct_Data *sd = pr.structs.e + i;

                write_ob(&ob,
                         "    else if(real_type == pp_Type_%.*s) {\n"
                         "        switch(index) {\n",
                         sd->name.len, sd->name.e);

                for(Int j = 0; (j < sd->member_count); ++j) {
                    Variable *mem = sd->members + j;

                    write_ob(&ob,
                             "            case %d: {\n"
                             "                pp_MemberDefinition res = {pp_Type_%.*s, \"%.*s\", PP_OFFSETOF(pp_%.*s, %.*s), %d, %d};\n"
                             "                return(res);\n"
                             "            } break; \n",
                             j,
                             mem->type.len, mem->type.e,
                             mem->name.len, mem->name.e,
                             sd->name.len, sd->name.e,
                             mem->name.len, mem->name.e,
                             mem->ptr,
                             mem->array_count);

                }

                write_ob(&ob,
                         "        }\n"
                         "    }\n");
            }


            write_ob(&ob,
                     "\n"
                     "    // Not found\n"
                     "    PP_ASSERT(0);\n"
                     "    PP_MEMSET(&failres, 0, sizeof(failres));\n"
                     "    return(failres);\n"
                     "}\n");
        }

        // Get number of members from type.
        {
            write_ob(&ob,
                     "\n"
                     "PP_STATIC uintptr_t pp_get_number_of_members(pp_Type type) {\n"
                     "    switch(pp_typedef_to_original(type)) {\n"
                     "        case pp_Type___m128: case pp_Type___m128i: { return(1); }\n");

            for(Int i = 0; (i < pr.structs.cnt); ++i) {
                Struct_Data *sd = pr.structs.e + i;

                write_ob(&ob,
                         "        case pp_Type_%.*s: { return(%d); } break;\n",
                         sd->name.len, sd->name.e, sd->member_count);

            }

            write_ob(&ob,
                     "    }\n");

            write_ob(&ob,
                     "\n"
                     "    PP_ASSERT(0);\n"
                     "    return(0);\n"
                     "}\n");
        }

        // Is primitive
        {
            write_ob(&ob,
                     "\n"
                     "typedef enum pp_StructureType {\n"
                     "    pp_StructureType_unknown,\n"
                     "\n"
                     "    pp_StructureType_primitive,\n"
                     "    pp_StructureType_struct,\n"
                     "    pp_StructureType_enum,\n"
                     "\n"
                     "    pp_StructureType_count\n"
                     "} pp_StructureType;\n"
                     "\n"
                     "PP_STATIC pp_StructureType pp_get_structure_type(pp_Type type) {\n"
                     "    switch(pp_typedef_to_original(type)) {\n");

            // Primitive.
            {
                write_ob(&ob, "        ");
                for(Int i = 0; (i < array_count(global_primitive_types)); ++i) {
                    write_ob(&ob, "case pp_Type_%s: ", global_primitive_types[i]);
                }
                write_ob(&ob, "case pp_Type_void: ");
                write_ob(&ob,
                         "{\n"
                         "            return(pp_StructureType_primitive);\n"
                         "        } break;\n");
            }

            // Enums
            if(pr.enums.cnt) {
                write_ob(&ob, "\n        ");
                for(Int i = 0; (i < pr.enums.cnt); ++i) {
                    Enum_Data *ed = pr.enums.e + i;

                    write_ob(&ob, "case pp_Type_%.*s: ", ed->name.len, ed->name.e);
                }
                write_ob(&ob,
                         "{\n"
                         "            return(pp_StructureType_enum);\n"
                         "        } break;\n");
            }

            // Structs.
            write_ob(&ob, "\n        case pp_Type___m128: case pp_Type___m128i: ");
            for(Int i = 0; (i < pr.structs.cnt); ++i) {
                Struct_Data *sd = pr.structs.e + i;

                write_ob(&ob, "case pp_Type_%.*s: ", sd->name.len, sd->name.e);
            }
            write_ob(&ob,
                     "{\n"
                     "            return(pp_StructureType_struct);\n"
                     "        } break;\n");

            write_ob(&ob,
                     "    }\n"
                     "\n"
                     "    PP_ASSERT(0);\n"
                     "    return(pp_StructureType_unknown);\n"
                     "}\n");
        }

        // Type to string.
        {
            write_ob(&ob,
                     "\n"
                     "PP_STATIC char const * pp_type_to_string(pp_Type type) {\n"
                     "    switch(type) {\n");
            for(Int i = 0; (i < type_information.count); ++i) {
                write_ob(&ob,
                         "        case pp_Type_%.*s: { return(\"%.*s\"); } break;\n",
                         type_information.types[i].len, type_information.types[i].e,
                         type_information.types[i].len, type_information.types[i].e);
            }

            write_ob(&ob,
                     "    }\n"
                     "    \n"
                     "    PP_ASSERT(0);\n"
                     "    return(0);\n"
                     "}\n");

        }

        // Get size from type.
        write_get_size_from_type(&ob, type_information.types, type_information.count, pr.typedefs, pr.enums);

        // Print struct.
        {
            write_to_output_buffer_no_var_args(&ob,
                                               "PP_STATIC char const * pp_enum_to_string(pp_Type type, intptr_t index);"
                                               "\n"
                                               "// uintptr_t pp_serialize_struct(TYPE *var, TYPE, buffer, buffer_size);\n"
                                               "#define pp_serialize_struct(var, Type, buf, size) pp_serialize_struct_type(var, PP_CONCAT(pp_Type_, Type), buf, size)\n"
                                               "#define pp_serialize_struct_type(var, Type, buf, size) pp_serialize_struct_(var, Type, PP_TO_STRING(var), 0, buf, size, 0)\n"
                                               "PP_STATIC uintptr_t\n"
                                               "pp_serialize_struct_(void *var, pp_Type type, char const *name, uintptr_t indent, char *buffer, uintptr_t buf_size, uintptr_t bytes_written) {\n"
                                               "    char indent_buf[256] = {0};\n"
                                               "    uintptr_t i, k, num_members;\n"
                                               "    int j;\n"
                                               "\n"
                                               "    PP_ASSERT((buffer) && (buf_size > 0)); // Check params.\n"
                                               "\n"
                                               "    // Setup the indent buffer.\n"
                                               "    indent += 4;\n"
                                               "    for(i = 0; (i < indent); ++i) {indent_buf[i] = ' ';}\n"
                                               "\n"
                                               "    num_members = pp_get_number_of_members(type); PP_ASSERT(num_members != -1); // Get the number of members for the for loop.\n"
                                               "    for(i = 0; (i < num_members); ++i) {\n"
                                               "        pp_MemberDefinition member = pp_get_members_from_type(type, i);\n"
                                               "        uintptr_t *member_ptr = (uintptr_t *)((char *)var + member.offset); // Get the actual pointer to the memory address.\n"
                                               "        pp_StructureType struct_type = pp_get_structure_type(member.type);\n"
                                               "        if(struct_type == pp_StructureType_primitive) {\n"
                                               "            char const *type_as_string = pp_type_to_string(member.type);\n"
                                               "            if(member.arr_size > 1) {\n"
                                               "                for(j = 0; (j < member.arr_size); ++j) {\n"
                                               "                    uintptr_t *member_ptr_as_uintptr = (uintptr_t *)member_ptr; // For arrays of pointers.\n"
                                               "                    pp_MyBool is_null = (member.ptr) ? member_ptr_as_uintptr[j] == 0 : PP_FALSE;\n"
                                               "                    if(!is_null) {\n"
                                               "\n"
                                               "#define print_prim_arr(m, Type, p) Type *v = (member.ptr) ? *(Type **)((char unsigned *)member_ptr + (sizeof(uintptr_t) * j)) : &((Type *)member_ptr)[j]; bytes_written += PP_SPRINTF((char *)buffer + bytes_written, buf_size - bytes_written, \"\\n%s \" #Type \" %s%s[%d] = \" m \"\", indent_buf, (member.ptr) ? \"*\" : \"\", member.name, j, p (Type *)v)\n"
                                               "                        pp_Type original_type = pp_typedef_to_original(member.type);\n"
                                               "                        if(original_type == pp_Type_double)     { print_prim_arr(\"%f\",  double, *); }\n"
                                               "                        else if(original_type == pp_Type_float) { print_prim_arr(\"%f\",  float, *);  }\n"
                                               "                        else if(original_type == pp_Type_int)   { print_prim_arr(\"%d\",  int, *);    }\n"
                                               "                        else if(original_type == pp_Type_long)  { print_prim_arr(\"%ld\", long, *);   }\n"
                                               "                        else if(original_type == pp_Type_short) { print_prim_arr(\"%d\",  short, *);  }\n"
                                               //"                        else if(original_type == pp_Type_bool)  { print_prim_arr(\"%d\",  int, *);    }\n"
                                               "\n"
                                               "                        else if(original_type == pp_Type_uint64_t) { print_prim_arr(\"%lu\", uint64_t, *); }\n"
                                               "                        else if(original_type == pp_Type_int64_t)  { print_prim_arr(\"%ld\", int64_t, *);  }\n"
                                               "                        else if(original_type == pp_Type_uint32_t) { print_prim_arr(\"%u\", uint32_t, *); }\n"
                                               "                        else if(original_type == pp_Type_int32_t)  { print_prim_arr(\"%d\", int32_t, *);  }\n"
                                               "                        else if(original_type == pp_Type_uint16_t) { print_prim_arr(\"%u\", uint16_t, *); }\n"
                                               "                        else if(original_type == pp_Type_int16_t)  { print_prim_arr(\"%d\", int16_t, *);  }\n"
                                               "                        else if(original_type == pp_Type_uint8_t) { print_prim_arr(\"%u\", uint8_t, *); }\n"
                                               "                        else if(original_type == pp_Type_int8_t)  { print_prim_arr(\"%d\", int8_t, *);  }\n"
                                               "\n"
                                               "                        //else if(original_type == pp_Type_intptr_t)  { print_prim_arr(\"%ld\", intptr_t, *);  }\n"
                                               "                        //else if(original_type == pp_Type_uintptr_t) { print_prim_arr(\"%lu\", uintptr_t, *); }\n"
                                               "                        else if(original_type == pp_Type_size_t)    { print_prim_arr(\"%zu\", size_t, *);    }\n"
                                               "\n"
                                               "                        else if(original_type == pp_Type_char) {\n"
                                               "                            if(member.ptr) {\n"
                                               "                                print_prim_arr(\"%s\",  char, /**/);\n"
                                               "                            } else {\n"
                                               "                                print_prim_arr(\"%c\",  char, *);\n"
                                               "                            }\n"
                                               "                        }\n"
                                               "#undef print_prim_arr\n"
                                               "                    } else {\n"
                                               "                        bytes_written += PP_SPRINTF((char *)buffer + bytes_written, buf_size - bytes_written, \"\\n%s %s %s%s[%d] = (null)\", indent_buf, type_as_string, (member.ptr) ? \"*\" : \"\", member.name, j);\n"
                                               "                    }\n"
                                               "                }\n"
                                               "            } else {\n"
                                               "                uintptr_t *v = 0;\n"
                                               "                if(member.ptr) {\n"
                                               "                    v = *(uintptr_t **)member_ptr;\n"
                                               "                    for(k = 0; (k < member.ptr - 1); ++k) {\n"
                                               "                        v = *(uintptr_t **)v;\n"
                                               "                    }\n"
                                               "                } else {\n"
                                               "                    v = (uintptr_t *)member_ptr;\n"
                                               "                }\n"
                                               "                if(v) {\n"
                                               "#define print_prim(m, Type, p) bytes_written += PP_SPRINTF((char *)buffer + bytes_written, buf_size - bytes_written, \"\\n%s \" #Type \" %s%s = \" m \"\", indent_buf, (member.ptr) ? \"*\" : \"\", member.name, p (Type *)v)\n"
                                               "                    pp_Type original_type = pp_typedef_to_original(member.type);\n"
                                               "                    if(original_type == pp_Type_double)     { print_prim(\"%f\",  double, *); }\n"
                                               "                    else if(original_type == pp_Type_float) { print_prim(\"%f\",  float, *);  }\n"
                                               "                    else if(original_type == pp_Type_int)   { print_prim(\"%d\",  int, *);    }\n"
                                               "                    else if(original_type == pp_Type_long)  { print_prim(\"%ld\", long, *);   }\n"
                                               "                    else if(original_type == pp_Type_short) { print_prim(\"%d\",  short, *);  }\n"
                                               //"                    else if(original_type == pp_Type_bool)  { print_prim(\"%d\",  int, *);    }\n"
                                               "\n"
                                               "                    else if(original_type == pp_Type_uint64_t)  { print_prim(\"%lu\", uint64_t, *); }\n"
                                               "                    else if(original_type == pp_Type_int64_t)   { print_prim(\"%ld\", int64_t, *);  }\n"
                                               "                    else if(original_type == pp_Type_uint32_t)  { print_prim(\"%u\", uint32_t, *); }\n"
                                               "                    else if(original_type == pp_Type_int32_t)   { print_prim(\"%d\", int32_t, *);  }\n"
                                               "                    else if(original_type == pp_Type_uint16_t)  { print_prim(\"%u\", uint16_t, *); }\n"
                                               "                    else if(original_type == pp_Type_int16_t)   { print_prim(\"%d\", int16_t, *);  }\n"
                                               "                    else if(original_type == pp_Type_uint8_t)   { print_prim(\"%u\", uint8_t, *);  }\n"
                                               "                    else if(original_type == pp_Type_int8_t)    { print_prim(\"%d\", int8_t, *);   }\n"
                                               "\n"
                                               "                    //else if(original_type == pp_Type_intptr_t)  { print_prim(\"%ld\", intptr_t, *);  }\n"
                                               "                    //else if(original_type == pp_Type_uintptr_t) { print_prim(\"%lu\", uintptr_t, *); }\n"
                                               "                    else if(original_type == pp_Type_size_t)    { print_prim(\"%zu\", size_t, *);    }\n"
                                               "\n"
                                               "                    else if(original_type == pp_Type_char) {\n"
                                               "                        if(member.ptr) {\n"
                                               "                            print_prim(\"%s\",  char, /**/);\n"
                                               "                        } else {\n"
                                               "                            print_prim(\"%c\",  char, *);\n"
                                               "                        }\n"
                                               "                    }\n"
                                               "#undef print_prim\n"
                                               "                } else {\n"
                                               "                    bytes_written += PP_SPRINTF((char *)buffer + bytes_written, buf_size - bytes_written, \"\\n%s %s *%s = (null)\", indent_buf, type_as_string, member.name);\n"
                                               "                }\n"
                                               "            }\n"
                                               "        } else if(struct_type == pp_StructureType_enum) {\n"
                                               "            char const *type_as_string = pp_type_to_string(member.type);\n"
                                               "            pp_Type original_type;\n"
                                               "            char const *enum_as_string;\n"
                                               "\n"
                                               "            int *v = 0; // Assuming the enum is an integer.\n"
                                               "            if(member.ptr) {\n"
                                               "                v = *(int **)member_ptr;\n"
                                               "                for(k = 0; (k < member.ptr - 1); ++k) {\n"
                                               "                    v = *(int **)v;\n"
                                               "                }\n"
                                               "            } else {\n"
                                               "                v = (int *)member_ptr;\n"
                                               "            }\n"
                                               "\n"
                                               "            original_type = pp_typedef_to_original(member.type);\n"
                                               "            enum_as_string = pp_enum_to_string(original_type, *(int *)v);\n"
                                               "            if(enum_as_string) {\n"
                                               "                bytes_written += PP_SPRINTF((char *)buffer + bytes_written, buf_size - bytes_written, \"\\n%s %s *%s = %s\", indent_buf, type_as_string, member.name, enum_as_string);\n"
                                               "            }\n"
                                               "\n"
                                               "        } else if(struct_type == pp_StructureType_struct) {\n"
                                               "            char tmp_arr_buf[64] = {0};\n"
                                               "            if(member.arr_size > 1) {\n"
                                               "                PP_SPRINTF(tmp_arr_buf, 32, \"[%d]\", (int)member.arr_size);\n"
                                               "            }\n"
                                               "\n"
                                               "            bytes_written += PP_SPRINTF((char *)buffer + bytes_written, buf_size - bytes_written, \"\\n%s%s %s%s%s\", indent_buf, pp_type_to_string(member.type), (member.ptr) ? \"*\" : \"\",member.name, tmp_arr_buf);\n"
                                               "            if(member.arr_size <= 1) {\n"
                                               "                void *ptr = (member.ptr) ? *(uintptr_t **)member_ptr : member_ptr;\n"
                                               "                bytes_written = pp_serialize_struct_(ptr, member.type, member.name, indent, buffer, buf_size - bytes_written, bytes_written);\n"
                                               "            } else {\n"
                                               "                for(j = 0; (j < member.arr_size); ++j) {\n"
                                               "                    uintptr_t size_of_struct = pp_get_size_from_type(member.type);\n"
                                               "\n"
                                               "                    char unsigned *ptr = 0;\n"
                                               "                    if(member.ptr) {\n"
                                               "                        ptr = *(char unsigned **)((char unsigned *)member_ptr + (j * sizeof(uintptr_t)));\n"
                                               "                    } else {\n"
                                               "                        ptr = ((char unsigned *)member_ptr + (j * size_of_struct));\n"
                                               "                    }\n"
                                               "\n"
                                               "                    bytes_written = pp_serialize_struct_(ptr, member.type, member.name, indent, buffer, buf_size - bytes_written, bytes_written);\n"
                                               "                }\n"
                                               "            }\n"
                                               "        }\n"
                                               "    }\n"
                                               "\n"
                                               "    buffer[bytes_written] = 0;\n"
                                               "    return(bytes_written);\n"
                                               "}\n");
        }

        // TODO(Jonny): This is a really nice feature, but it doesn't work yet because it requires a pp_Thingy, and not a Thingy.
        //              Maybe I could solve this by putting a constructor into each generated struct that means they can be
        //              quickly converted?
        // Struct compare.
#if 0
        write_ob(&ob,
                 "//\n"
                 "// Generated comparisons.\n"
                 "//\n");

        for(Int i = 0; (i < pr.structs.cnt); ++i) {
            Struct_Data *sd = pr.structs.e + i;

            write_ob(&ob,
                     "PP_STATIC pp_MyBool operator==(pp_%.*s a, pp_%.*s b) {\n",
                     sd->name.len, sd->name.e,
                     sd->name.len, sd->name.e);

            for(Int j = 0; (j < sd->member_count); ++j) {
                Variable *mem = sd->members + j;

                if(mem->array_count) { continue; } // TODO(Jonny): Don't support arrays yet.

                write_ob(&ob,
                         "    if(a.%.*s != b.%.*s) { return(false); }\n",
                         mem->name.len, mem->name.e,
                         mem->name.len, mem->name.e);
            }

            write_ob(&ob,
                     "\n"
                     "    return(true);\n"
                     "}\n");
        }
#endif

        write_enum_size_data(&ob, pr.enums);

        {
            write_ob(&ob,
                     "\n"
                     "//\n"
                     "// String to enum.\n"
                     "//\n"
                     "PP_STATIC intptr_t pp_string_to_enum(pp_Type type, char const *str) {\n");

            if(pr.enums.cnt) {
                write_ob(&ob, "    switch(pp_typedef_to_original(type)) {\n");

                for(Int i = 0; (i < pr.enums.cnt); ++i) {
                    Enum_Data *ed = pr.enums.e + i;

                    write_ob(&ob,
                             "        case pp_Type_%.*s: {\n",
                             ed->name.len, ed->name.e);

                    for(Int j = 0; (j < ed->no_of_values); ++j) {
                        Enum_Value *ev = ed->values + j;

                        if(!j) { write_ob(&ob, "            "); }
                        else { write_ob(&ob, "            else "); }

                        write_ob(&ob,
                                 "if(pp_string_compare(str, \"%.*s\")) { return(%d); }\n",
                                 ev->name.len, ev->name.e, ev->value);
                    }

                    write_ob(&ob,
                             "        } break;\n");

                }

                write_ob(&ob,
                         "    }\n");
            }

            write_ob(&ob,
                     "\n"
                     "    PP_ASSERT(0);\n"
                     "    return(0);\n"
                     "}\n");
        }

        // Enum to string.
        {
            write_ob(&ob,
                     "\n"
                     "//\n"
                     "// Enum to string.\n"
                     "//\n"
                     "PP_STATIC char const * pp_enum_to_string(pp_Type type, intptr_t index) {\n");
            if(pr.enums.cnt) {
                write_ob(&ob, "    switch(pp_typedef_to_original(type)) {\n");
                for(Int i = 0; (i < pr.enums.cnt); ++i) {
                    Enum_Data *ed = pr.enums.e + i;

                    write_ob(&ob,
                             "        case pp_Type_%.*s: {\n"
                             "            switch(index) {\n",
                             ed->name.len, ed->name.e);

                    for(Int j = 0; (j < ed->no_of_values); ++j) {
                        Enum_Value *ev = ed->values + j;

                        write_ob(&ob,
                                 "                case %d: { return(\"%.*s\"); } break;\n",
                                 ev->value, ev->name.len, ev->name.e);
                    }

                    write_ob(&ob,
                             "            }\n"
                             "        } break;\n");

                }

                write_ob(&ob,
                         "    }\n");
            }

            write_ob(&ob,
                     "\n"
                     "    PP_ASSERT(0);\n"
                     "    return(0);\n"
                     "}\n");
        }
    }

    //
    // # Guard macro.
    //
    write_ob(&ob,
             "\n"
             "#if defined(__cplusplus)\n"
             "} // extern \"C\"\n"
             "#endif\n"
             "\n"
             "#define PP_GENERATED_H\n"
             "#endif // #if defined(PP_GENERATED_H)\n"
             "\n");

    res.size = ob.index;
    res.e = ob.buffer;

    system_free(type_information.types);



    return(res);
}
