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

internal Void write(OutputBuffer *ob, Char *format, ...) {
    va_list args;
    va_start(args, format);
    ob->index += stbsp_vsnprintf(ob->buffer + ob->index, ob->size - ob->index, format, args);
    assert(ob->index < ob->size);
    va_end(args);
}

internal Void write_to_output_buffer_no_var_args(OutputBuffer *ob, Char *format) {
    Char *fat = format;
    Char *obat = ob->buffer + ob->index;
    while(*fat) {
        ++ob->index;
        *obat++ = *fat++;
    }
}

internal Char *primitive_types[] = {"char", "short", "int", "long", "float", "double", "bool"};

internal StructData *find_struct(String str, Structs structs) {
    StructData *res = 0;

    if(str.len) {
        for(Int i = 0; (i < structs.cnt); ++i) {
            StructData *sd = structs.e + i;

            if(string_comp(str, sd->name)) {
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
        if(string_comp(array[i], test)) {
            res = true;
            break;
        }
    }

    return(res);
}

internal Int get_actual_type_count(String *types, Structs structs, Enums enums, Typedefs typedefs) {
    Int res = 0;

    // Primitives.
    for(Int i = 0; (i < array_count(primitive_types)); ++i) {
        types[res++] = create_string(primitive_types[i]);
    }

    // Typedefs.
    for(Int i = 0; (i < typedefs.cnt); ++i) {
        TypedefData *td = typedefs.e + i;

        if(!is_meta_type_already_in_array(types, res, td->fresh)) {
            types[res++] = td->fresh;
        }
    }

    // Enums
    for(Int i = 0; (i < enums.cnt); ++i) {
        EnumData *ed = enums.e + i;

        if(!is_meta_type_already_in_array(types, res, ed->name)) {
            types[res++] = ed->name;
        }
    }

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

File write_data(ParseResult pr) {
    File res = {};

    OutputBuffer ob = {};
    ob.size = 1024 * 1024;
    ob.buffer = system_alloc(Char, ob.size);
    if(ob.buffer) {
        write(&ob,
              "#if !defined(PP_GENERATED_H)\n"
              "\n"
              "typedef void pp_void;\n"
              "typedef char pp_char;\n"
              "typedef short pp_short;\n"
              "typedef int pp_int;\n"
              "typedef long pp_long;\n"
              "typedef float pp_float;\n"
              "typedef double pp_double;\n"
              "#if defined(__cplusplus)\n"
              "    typedef bool pp_bool;\n"
              "    #define PP_TRUE true\n"
              "    #define PP_FALSE false\n"
              "#else\n"
              "    typedef int pp_bool;\n"
              "    #define PP_TRUE 1\n"
              "    #define PP_FALSE 0\n"
              "#endif\n"
              "\n"
              "// Standard lib stuff.\n"
              "#if !defined(PP_ASSERT)\n"
              "    #include <assert.h>\n"
              "    #define PP_ASSERT assert\n"
              "#endif\n"
              "\n"
              "#define PP_CONCAT(a, b) a##b\n"
              "#define PP_TO_STRING(a) #a\n"
              "\n"
              "#define PP_OFFSETOF(T, var) ((size_t)&(((T *)0)->var))\n"
              "\n"
              "#if !defined(PP_SPRINTF)\n"
              "    #if defined(_MSC_VER)\n"
              "        #define PP_SPRINTF(buf, size, format, ...) sprintf_s(buf, size, format, ##__VA_ARGS__)\n"
              "    #else\n"
              "        #define PP_SPRINTF(buf, size, format, ...) sprintf(buf, format, ##__VA_ARGS__)\n"
              "    #endif\n"
              "#endif\n"
              "\n"
              "#if defined(PP_STATIC_FUNCS)\n"
              "    #define PP_STATIC static\n"
              "#else\n"
              "    #define PP_STATIC\n"
              "#endif\n"
              "\n"
              "PP_STATIC pp_bool pp_string_compare(char const *a, char const *b) {\n"
              "    for(;; ++a, ++b) {\n"
              "        if((*a == 0) && (*b == 0)) {\n"
              "            return(PP_TRUE);\n"
              "        } else if(*a != *b) {\n"
              "            return(PP_FALSE);\n"
              "        }\n"
              "    }\n"
              "}\n");

        // Create typedefs
        {
            write(&ob,
                  "\n"
                  "//\n"
                  "// Create typedefs.\n"
                  "//\n");

            for(Int i = 0; (i < pr.typedefs.cnt); ++i) {
                TypedefData *td = pr.typedefs.e + i;

                write(&ob,
                      "typedef pp_%.*s pp_%.*s;\n",
                      td->original.len, td->original.e,
                      td->fresh.len, td->fresh.e);
            }
        }

        // Forward declared structs/functions.
        {
            write(&ob,
                  "\n"
                  "//\n"
                  "// Forward declared structs, enums, and functions\n"
                  "//\n");

            // Forward declare structs.
            for(Int i = 0; (i < pr.structs.cnt); ++i) {
                StructData *sd = pr.structs.e + i;

                if(sd->struct_type == StructType_struct) {
                    write(&ob, "struct %.*s;\n", sd->name.len, sd->name.e);
                } else if(sd->struct_type == StructType_class) {
                    write(&ob, "class %.*s;\n", sd->name.len, sd->name.e);
                } else if(sd->struct_type == StructType_union) {
                    write(&ob, "union %.*s;\n", sd->name.len, sd->name.e);
                } else {
                    assert(0);
                }
            }

            // Forward declared functions.
            for(Int i = 0; (i < pr.funcs.cnt); ++i) {
                FunctionData *fd = pr.funcs.e + i;

                Char ptr_buf[max_ptr_size] = {};
                for(Int j = 0; (j < fd->return_type_ptr); ++j) {
                    ptr_buf[j] = '*';
                }

                write(&ob,
                      "%.*s %.*s%s %.*s(",
                      fd->linkage.len, fd->linkage.e,
                      fd->return_type.len, fd->return_type.e,
                      ptr_buf,
                      fd->name.len, fd->name.e);

                for(Int j = 0; (j < fd->param_cnt); ++j) {
                    Variable *param = fd->params + j;

                    Char param_ptr_buf[max_ptr_size] = {};
                    for(Int k = 0; (k < param->ptr); ++k) {
                        param_ptr_buf[k] = '*';
                    }

                    if(j) {
                        write(&ob, ", ");
                    }

                    write(&ob,
                          "%.*s %s %.*s",
                          param->type.len, param->type.e,
                          param_ptr_buf,
                          param->name.len, param->name.e);
                }

                write(&ob, ");\n");
            }
        }

        Int max_type_count = array_count(primitive_types);
        for(Int i = 0; (i < pr.structs.cnt); ++i) {
            max_type_count += pr.structs.e[i].member_count + 1;
        }

        Int type_count = 0;
        String *types = system_alloc(String, max_type_count);
        if(types) {

            // Meta types enum.
            {
                type_count = get_actual_type_count(types, pr.structs, pr.enums, pr.typedefs);

                write(&ob, "enum pp_Type {\n");

                for(Int i =0; (i < type_count); ++i) {
                    String *t = types + i;

                    write(&ob,
                          "    pp_Type_%.*s,\n",
                          t->len, t->e);
                }

                write(&ob, "};\n");

                //
                // Dynamic array.
                //
                {
                    write(&ob,
                          "\n"
                          "//\n"
                          "// Dynamic Array.\n"
                          "//\n"
                          "#define pp_DynamicArray(Type) PP_CONCAT(pp_DynamicArray_, Type)\n"
                          "\n");

                    write(&ob,
                          "typedef struct pp_DynamicArray_void { "
                          " pp_int capacity, size;"
                          " void *e;"
                          " } pp_DynamicArray_void, pp_pp_DynamicArray_void;\n");


                    // Primitives.
                    for(Int i = 0; (i < array_count(primitive_types)); ++i) {
                        Char *p = primitive_types[i];

                        write(&ob,
                              "typedef struct pp_DynamicArray_%s { "
                              " pp_int capacity, size;"
                              " %s *e;"
                              " } pp_DynamicArray_%s, pp_pp_DynamicArray_%s;\n",
                              p, p, p, p);
                    }

                    // Structs.
                    for(Int i = 0; (i < pr.structs.cnt); ++i) {
                        StructData *sd = pr.structs.e + i;

                        write(&ob,
                              "typedef struct pp_DynamicArray_%.*s {"
                              " pp_int capacity, size;"
                              " %.*s *e;"
                              "} pp_DynamicArray_%.*s, pp_pp_DynamicArray_%.*s;\n",
                              sd->name.len, sd->name.e,
                              sd->name.len, sd->name.e,
                              sd->name.len, sd->name.e,
                              sd->name.len, sd->name.e);

                    }
                }

                //
                // Recreated structs.
                //
                {
                    write(&ob,
                          "\n"
                          "//\n"
                          "// Recreated structs.\n"
                          "//\n");
                    for(Int i = 0; (i < pr.structs.cnt); ++i) {
                        StructData *sd = pr.structs.e + i;

                        write(&ob, "%s pp_%.*s", (sd->struct_type != StructType_union) ? "struct" : "union",
                              sd->name.len, sd->name.e);

                        write(&ob, " { ");

                        Bool is_inside_anonymous_struct = false;
                        for(Int j = 0; (j < sd->member_count); ++j) {
                            Variable *md = sd->members + j;

                            if(md->is_inside_anonymous_struct != is_inside_anonymous_struct) {
                                is_inside_anonymous_struct = !is_inside_anonymous_struct;

                                if(is_inside_anonymous_struct) {
                                    write(&ob, " struct {");
                                } else {
                                    write(&ob, "};");
                                }
                            }

                            Char *arr = "";
                            Char arr_buffer[256] = {};
                            if(md->array_count > 1) {
                                stbsp_snprintf(arr_buffer, 256, "[%u]", md->array_count);
                                arr = arr_buffer;
                            }

                            char ptr_buf[max_ptr_size] = {};
                            for(Int k = 0; (k < md->ptr); ++k) {
                                ptr_buf[k] = '*';
                            }

                            write(&ob, " pp_%.*s %s%.*s%s; ",
                                  md->type.len, md->type.e,
                                  ptr_buf,
                                  md->name.len, md->name.e,
                                  (md->array_count > 1) ? arr_buffer : arr);

                        }

                        if(is_inside_anonymous_struct) {
                            write(&ob, " };");
                        }

                        write(&ob, " };\n");
                    }
                }
            }

            //
            // Dynamic array functions
            //
            {
                write(&ob,
                      "\n"
                      "//\n"
                      "// Dynamic Array functions.\n"
                      "//\n"
                      "\n"
                      "// Remove element from end of array, and return it by value.\n"
                      "// TYPE pp_pop(pp_DynamicArray(TYPE) *da);\n"
                      "#define pp_pop(da) (--(da)->size, (da)->e[(da)->size]);\n"
                      "\n"
                      "// Add an element onto the end of the array, and return a pointer to it.\n"
                      "// TYPE *pp_push(pp_DynamicArray(TYPE) *da, TYPE v);\n"
                      "#define pp_push(da, v) (pp_dynamic_array_grow_if_needed((pp_DynamicArray(void) *)(da), sizeof(v)), (da)->e[(da)->size++] = (v), &((da)->e[(da)->size - 1]))\n"
                      "\n"
                      "// Add an element at index i in the array, and return a pointer to it.\n"
                      "// TYPE *pp_insert(pp_DynamicArray(TYPE) *da, TYPE v, int i);\n"
                      "#define pp_insert(da, v, i) (pp_dynamic_array_grow_if_needed((pp_DynamicArray(void) *)(da), sizeof(v)), memmove((da)->e + (i) + 1, (da)->e + (i), (++(da)->size - (i)) * sizeof(*(da)->e)), (da)->e[i] = (v), &(da)->e[(i)])\n"
                      "\n"
                      "// Remove an element from the array.\n"
                      "// void pp_remove(pp_DynamicArray(TYPE), int i);\n"
                      "#define pp_remove(da, i) (memmove((da)->e + (i), (da)->e + (i) + 1, (--(da)->size - (i)) * sizeof(*(da)->e)))\n"
                      "\n"
                      "// Make the array grow by one, if needed.\n"
                      "PP_STATIC void pp_dynamic_array_grow_if_needed(pp_DynamicArray(void) *da, int stride) {\n"
                      "    if(!da->capacity) {\n"
                      "        da->e = malloc(4 * stride);\n"
                      "        if(da->e) {\n"
                      "            da->capacity = 4;\n"
                      "        } else {\n"
                      "            // No Memory\n"
                      "        }\n"
                      "    } else if(da->size + 1 >= da->capacity) {\n"
                      "        int new_capacity = da->capacity * 2;\n"
                      "        void *p = realloc(da->e, new_capacity * stride);\n"
                      "        if(p) {\n"
                      "            da->e = p;\n"
                      "            da->capacity = new_capacity;\n"
                      "        } else {\n"
                      "            // No memory.\n"
                      "        }\n"
                      "    }\n"
                      "}\n");
            }

            // Typedef to origional.
            {
                write(&ob,
                      "\n"
                      "PP_STATIC pp_Type pp_typedef_to_original(pp_Type type) {\n"
                      "    switch(type) {\n");

                for(Int i = 0; (i < pr.typedefs.cnt); ++i) {
                    TypedefData *td = pr.typedefs.e + i;

                    write(&ob,
                          "        case pp_Type_%.*s: { return(pp_Type_%.*s); } break;\n",
                          td->fresh.len, td->fresh.e,
                          td->original.len, td->original.e);
                }

                write(&ob,
                      "    }\n"
                      "\n"
                      "    return(type);\n"
                      "}\n");
            }

            // get members from type.
            {
                write(&ob,
                      "struct pp_MemberDefinition {\n"
                      "    pp_Type type;\n"
                      "    char const *name;\n"
                      "    size_t offset;\n"
                      "    int ptr;\n"
                      "    int arr_size;\n"
                      "};\n"
                      "\n"
                      "PP_STATIC pp_MemberDefinition pp_get_members_from_type(pp_Type type, pp_int i) {\n"
                      "    pp_Type real_type = pp_typedef_to_original(type);\n");
                for(Int i = 0; (i < pr.structs.cnt); ++i) {
                    StructData *sd = pr.structs.e + i;

                    if(!i) { write(&ob, "    ");      }
                    else   { write(&ob, "    else "); }

                    write(&ob,
                          "if(real_type == pp_Type_%.*s) {\n"
                          "        switch(i) {\n",
                          sd->name.len, sd->name.e);

                    for(Int j = 0; (j < sd->member_count); ++j) {
                        Variable *mem = sd->members + j;

                        write(&ob,
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

                    write(&ob,
                          "        }\n"
                          "    }\n");
                }


                write(&ob,
                      "    // Not found\n"
                      "    PP_ASSERT(0);\n"
                      "    pp_MemberDefinition res = {};\n"
                      "    return(res);\n"
                      "}\n");
            }

            // Get number of members from type.
            {
                write(&ob,
                      "\n"
                      "PP_STATIC int pp_get_number_of_members(pp_Type type) {\n"
                      "    switch(pp_typedef_to_original(type)) {\n");

                for(Int i = 0; (i < pr.structs.cnt); ++i) {
                    StructData *sd = pr.structs.e + i;

                    write(&ob,
                          "        case pp_Type_%.*s: { return(%d); } break;\n",
                          sd->name.len, sd->name.e, sd->member_count);

                }

                write(&ob,
                      "    }\n"
                      "\n"
                      "    PP_ASSERT(0);\n"
                      "    return(0);\n"
                      "}\n");
            }

            // Is primitive
            {
                write(&ob,
                      "\n"
                      "enum pp_StructureType {\n"
                      "    pp_StructureType_unknown,\n"
                      "    pp_StructureType_primitive,\n"
                      "    pp_StructureType_struct,\n"
                      "    pp_StructureType_enum,\n"
                      "    pp_StructureType_count,\n"
                      "};\n"
                      "\n"
                      "PP_STATIC pp_StructureType pp_get_structure_type(pp_Type type) {\n"
                      "    switch(pp_typedef_to_original(type)) {\n"
                      "        ");

                // Primitive.
                {
                    for(Int i = 0; (i < array_count(primitive_types)); ++i) {
                        write(&ob, "case pp_Type_%s: ", primitive_types[i]);
                    }
                    write(&ob,
                          " {\n"
                          "            return(pp_StructureType_primitive);\n"
                          "        } break;\n"
                          "        ");
                }

                // Enums
                {
                    for(Int i = 0; (i < pr.enums.cnt); ++i) {
                        EnumData *ed = pr.enums.e + i;

                        write(&ob, "case pp_Type_%.*s: ", ed->name.len, ed->name.e);
                    }
                    write(&ob,
                          " {\n"
                          "            return(pp_StructureType_enum);\n"
                          "        } break;\n");
                }


                // Structs.
                {
                    for(Int i = 0; (i < pr.structs.cnt); ++i) {
                        StructData *sd = pr.structs.e + i;

                        write(&ob, "case pp_Type_%.*s: ", sd->name.len, sd->name.e);
                    }
                    write(&ob,
                          " {\n"
                          "            return(pp_StructureType_struct);\n"
                          "        } break;\n");
                }

                write(&ob,
                      "    }\n"
                      "\n"
                      "    PP_ASSERT(0);\n"
                      "    return(pp_StructureType_unknown);\n"
                      "}\n");
            }

            // Type to string.
            {
                write(&ob,
                      "\n"
                      "PP_STATIC char const * pp_type_to_string(pp_Type type) {\n"
                      "    switch(type) {\n");

                for(Int i = 0; (i < array_count(primitive_types)); ++i) {
                    Char *prim = primitive_types[i];

                    write(&ob,
                          "        case pp_Type_%s: { return(\"%s\"); } break;\n",
                          prim, prim);
                }

                for(Int i = 0; (i < pr.structs.cnt); ++i) {
                    StructData *sd = pr.structs.e + i;

                    write(&ob,
                          "        case pp_Type_%.*s: { return(\"%.*s\"); } break;\n",
                          sd->name.len, sd->name.e,
                          sd->name.len, sd->name.e);
                }

                for(Int i = 0; (i < pr.typedefs.cnt); ++i) {
                    TypedefData *td = pr.typedefs.e + i;

                    write(&ob,
                          "        case pp_Type_%.*s: { return(\"%.*s\"); } break;\n",
                          td->fresh.len, td->fresh.e,
                          td->fresh.len, td->fresh.e);
                }

                for(Int i = 0; (i < pr.enums.cnt); ++i) {
                    EnumData *ed = pr.enums.e + i;

                    write(&ob,
                          "        case pp_Type_%.*s: { return(\"%.*s\"); } break;\n",
                          ed->name.len, ed->name.e,
                          ed->name.len, ed->name.e);
                }

                write(&ob,
                      "    }\n"
                      "    \n"
                      "    PP_ASSERT(0);\n"
                      "    return(0);\n"
                      "}\n");

            }

            // Get size from type.
            {
                write(&ob,
                      "\n"
                      "PP_STATIC size_t pp_get_size_from_type(pp_Type _type) {\n"
                      "    pp_Type type = pp_typedef_to_original(_type);\n"
                      "\n"
                      "    switch(type) {\n");

                for(Int i = 0; (i < pr.structs.cnt); ++i) {
                    StructData *sd = pr.structs.e + i;

                    write(&ob,
                          "        case pp_Type_%.*s: { return(sizeof(pp_%.*s)); } break;\n",
                          sd->name.len, sd->name.e,
                          sd->name.len, sd->name.e);
                }

                write(&ob,
                      "    }\n"
                      "\n"
                      "    PP_ASSERT(0);\n"
                      "    return(0);\n"
                      "}\n");
            }

            // Print struct.
            {
                write_to_output_buffer_no_var_args(&ob,
                                                   "\n"
                                                   "#define pp_serialize_struct(var, Type, buf, size) pp_serialize_struct_(var, PP_CONCAT(pp_Type_, Type), PP_TO_STRING(var), 0, buf, size, 0)\n"
                                                   "PP_STATIC size_t\n"
                                                   "pp_serialize_struct_(void *var, pp_Type type, char const *name, int indent, char *buffer, size_t buf_size, size_t bytes_written) {\n"
                                                   "    PP_ASSERT((buffer) && (buf_size > 0)); // Check params.\n"
                                                   "\n"
                                                   "    // Setup the indent buffer.\n"
                                                   "    char indent_buf[256] = {0};\n"
                                                   "    indent += 4;\n"
                                                   "    for(int i = 0; (i < indent); ++i) {indent_buf[i] = ' ';}\n"
                                                   "\n"
                                                   "    int num_members = pp_get_number_of_members(type); PP_ASSERT(num_members != -1); // Get the number of members for the for loop.\n"
                                                   "    for(int i = 0; (i < num_members); ++i) {\n"
                                                   "        pp_MemberDefinition member = pp_get_members_from_type(type, i);\n"
                                                   "        size_t *member_ptr = (size_t *)((char *)var + member.offset); // Get the actual pointer to the memory address.\n"
                                                   "        pp_StructureType struct_type = pp_get_structure_type(member.type);\n"
                                                   "        if(struct_type == pp_StructureType_primitive) {\n"
                                                   "            char const *type_as_string = pp_type_to_string(member.type);\n"
                                                   "            if(member.arr_size > 1) {\n"
                                                   "                for(int j = 0; (j < member.arr_size); ++j) {\n"
                                                   "                    size_t *member_ptr_as_size_t = (size_t *)member_ptr; // For arrays of pointers.\n"
                                                   "                    pp_bool is_null = (member.ptr) ? member_ptr_as_size_t[j] == 0 : PP_FALSE;\n"
                                                   "                    if(!is_null) {\n"
                                                   "\n"
                                                   "#define print_prim_arr(m, Type, p) Type *v = (member.ptr) ? *(Type **)((char unsigned *)member_ptr + (sizeof(size_t) * j)) : &((Type *)member_ptr)[j]; bytes_written += PP_SPRINTF((char *)buffer + bytes_written, buf_size - bytes_written, \"\\n%s \" #Type \" %s%s[%d] = \" m \"\", indent_buf, (member.ptr) ? \"*\" : \"\", member.name, j, p (Type *)v)\n"
                                                   "                        pp_Type original_type = pp_typedef_to_original(member.type);\n"
                                                   "                        if(original_type == pp_Type_double)     { print_prim_arr(\"%f\",  double, *); }\n"
                                                   "                        else if(original_type == pp_Type_float) { print_prim_arr(\"%f\",  float, *);  }\n"
                                                   "                        else if(original_type == pp_Type_int)   { print_prim_arr(\"%d\",  int, *);    }\n"
                                                   "                        else if(original_type == pp_Type_long)  { print_prim_arr(\"%ld\", long, *);   }\n"
                                                   "                        else if(original_type == pp_Type_short) { print_prim_arr(\"%d\",  short, *);  }\n"
                                                   "                        else if(original_type == pp_Type_bool)  { print_prim_arr(\"%d\",  int, *);    }\n"
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
                                                   "                size_t *v;\n"
                                                   "                if(member.ptr) {\n"
                                                   "                    v = *(size_t **)member_ptr;\n"
                                                   "                    for(int i = 0; (i < member.ptr - 1); ++i) {\n"
                                                   "                        v = *(size_t **)v;\n"
                                                   "                    }\n"
                                                   "                } else {\n"
                                                   "                    v= (size_t *)member_ptr;\n"
                                                   "                }\n"
                                                   "                if(v) {\n"
                                                   "#define print_prim(m, Type, p) bytes_written += PP_SPRINTF((char *)buffer + bytes_written, buf_size - bytes_written, \"\\n%s \" #Type \" %s%s = \" m \"\", indent_buf, (member.ptr) ? \"*\" : \"\", member.name, p (Type *)v)\n"
                                                   "                    pp_Type original_type = pp_typedef_to_original(member.type);\n"
                                                   "                    if(original_type == pp_Type_double)     { print_prim(\"%f\",  double, *); }\n"
                                                   "                    else if(original_type == pp_Type_float) { print_prim(\"%f\",  float, *);  }\n"
                                                   "                    else if(original_type == pp_Type_int)   { print_prim(\"%d\",  int, *);    }\n"
                                                   "                    else if(original_type == pp_Type_long)  { print_prim(\"%ld\", long, *);   }\n"
                                                   "                    else if(original_type == pp_Type_short) { print_prim(\"%d\",  short, *);  }\n"
                                                   "                    else if(original_type == pp_Type_bool)  { print_prim(\"%d\",  int, *);    }\n"
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
                                                   "// Unsupported, for now.\n"
                                                   "        } else if(struct_type == pp_StructureType_struct) {\n"
                                                   "            char tmp_arr_buf[32] = {0};\n"
                                                   "            if(member.arr_size > 1) {\n"
                                                   "                PP_SPRINTF(tmp_arr_buf, 32, \"[%d]\", member.arr_size);\n"
                                                   "            }\n"
                                                   "            bytes_written += PP_SPRINTF((char *)buffer + bytes_written, buf_size - bytes_written, \"\\n%s%s %s%s%s\", indent_buf, pp_type_to_string(member.type), (member.ptr) ? \"*\" : \"\",member.name, tmp_arr_buf);\n"
                                                   "            if(member.arr_size <= 1) {\n"
                                                   "                void *ptr = (member.ptr) ? *(size_t **)member_ptr : member_ptr;\n"
                                                   "                bytes_written = pp_serialize_struct_(ptr, member.type, member.name, indent, buffer, buf_size - bytes_written, bytes_written);\n"
                                                   "            } else {\n"
                                                   "                for(int j = 0; (j < member.arr_size); ++j) {\n"
                                                   "                    size_t size_of_struct = pp_get_size_from_type(member.type);\n"
                                                   "\n"
                                                   "                    char unsigned *ptr;\n"
                                                   "                    if(member.ptr) {\n"
                                                   "                        ptr = *(char unsigned **)((char unsigned *)member_ptr + (j * sizeof(size_t)));\n"
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

            // Enum size.
            {
                write(&ob,
                      "\n"
                      "//\n"
                      "// Number of members in an enum.\n"
                      "//\n"
                      "PP_STATIC size_t pp_get_enum_size(pp_Type type) {\n"
                      "    switch(pp_typedef_to_original(type)) {\n");

                for(Int i = 0; (i < pr.enums.cnt); ++i) {
                    EnumData *ed = pr.enums.e + i;

                    write(&ob,
                          "        case pp_Type_%.*s: { return(%d); } break;\n",
                          ed->name.len, ed->name.e,
                          ed->no_of_values);
                }

                write(&ob,
                      "    }\n"
                      "\n"
                      "    PP_ASSERT(0);\n"
                      "    return(0);\n"
                      "}\n");
            }

            // String to enum.
            {
                write(&ob,
                      "\n"
                      "//\n"
                      "// String to enum.\n"
                      "//\n"
                      "PP_STATIC int pp_string_to_enum(pp_Type type, char const *str) {\n"
                      "    switch(pp_typedef_to_original(type)) {\n");

                for(Int i = 0; (i < pr.enums.cnt); ++i) {
                    EnumData *ed = pr.enums.e + i;

                    write(&ob,
                          "        case pp_Type_%.*s: {\n",
                          ed->name.len, ed->name.e);

                    for(Int j = 0; (j < ed->no_of_values); ++j) {
                        EnumValue *ev = ed->values + j;

                        if(!j) { write(&ob, "            ");      }
                        else   { write(&ob, "            else "); }

                        write(&ob,
                              "if(pp_string_compare(str, \"%.*s\")) { return(%d); }\n",
                              ev->name.len, ev->name.e, ev->value);
                    }

                    write(&ob,
                          "        } break;\n");

                }

                write(&ob,
                      "    }\n"
                      "\n"
                      "    PP_ASSERT(0);\n"
                      "    return(0);\n"
                      "}\n");
            }

            // Enum to string.
            {
                write(&ob,
                      "\n"
                      "//\n"
                      "// Enum to string.\n"
                      "//\n"
                      "PP_STATIC char const * pp_enum_to_string(pp_Type type, int i) {\n"
                      "    switch(pp_typedef_to_original(type)) {\n");
                for(Int i = 0; (i < pr.enums.cnt); ++i) {
                    EnumData *ed = pr.enums.e + i;

                    write(&ob,
                          "        case pp_Type_%.*s: {\n"
                          "            switch(i) {\n",
                          ed->name.len, ed->name.e);

                    for(Int j = 0; (j < ed->no_of_values); ++j) {
                        EnumValue *ev = ed->values + j;

                        write(&ob,
                              "                case %d: { return(\"%.*s\"); } break;\n",
                              ev->value, ev->name.len, ev->name.e);
                    }

                    write(&ob,
                          "            }\n"
                          "        } break;\n");

                }

                write(&ob,
                      "    }\n"
                      "\n"
                      "    PP_ASSERT(0);\n"
                      "    return(0);\n"
                      "}\n");
            }

            system_free(types);
        }

        //
        // # Guard macro.
        //
        write(&ob,
              "\n"
              "#define PP_GENERATED_H\n"
              "#endif // #if defined(PP_GENERATED_H)\n"
              "\n");

        res.size = ob.index;
        res.e = ob.buffer;
    }

    return(res);
}
