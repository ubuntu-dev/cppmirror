/*===================================================================================================
  File:                    main.cpp
  Author:                  Jonathan Livingstone
  Email:                   seagull127@ymail.com
  Licence:                 Public Domain
                           No Warranty is offered or implied about the reliability,
                           suitability, or usability
                           The use of this code is at your own risk
                           Anyone can use this code, modify it, sell it to terrorists, etc.
  ===================================================================================================*/

/* TODO(Jonny):
    - struct to tuple.
    - std::map, std::unordered_map, std::set, std::array.
    - Struct meta data.
        - Have a way to test if a member is private or not.
        - An alternative to pp::print and pp::serialize that hide private members.
        - It breaks if you use a comma to declare members of the same type.
        - Have a way to get the type of different elements (as strings or types).
        - Get a get_member(v, i) function, which simple returns the member at index i.
        - Output a _useful_ error message if the user declares the same struct twice.
        - In serialize struct, if there is a member which is an enum, call enum_to_string on it's value.
        - Create bool is_primitive(T) which returns if something is a primitive or not.
    - Union meta data.
        - Simple version of struct.
    - Function meta data.
        - Get name (as string).
        - Get linkage (as string).
        - Get return type.
        - Get param count.
        - Get params types, and names as strings.
    - References.
    - Templates.
    - Allow mathematical macros (1 + 1) to be the index for an array.
    - Global consts for arrays.
    - Handle typedefs.
    - Base type macro. If the programmer enters non-pointer (or non reference) value, just return the same value.
    - Make a is_primitive function.
    - Make a function tell if something's a pointer or not. Could return false if not a pointer, and a positive integer
      for the level of pointer otherwise. Should work with references too.
    - If the user puts a directory in front of the file name ("dir/source.cpp") then the outputted code will get
      placed in the directories parent, not the directory with the code.
    - I could handle user-defined containers, assuming they conform to some strict set of rules. These could be:
        - Does it implement the nessessary member functions to use the C++11 range-based for loops?
        - It it a template struct where the template can only take one type?
    - I don't think #if 1 #else blocks work correctly...
    - A function which gets a pointer to a base class, from a normal class?
*/

#include "utils.h"
#include "lexer.h"
#include "platform.h"
#include "write_file.h"
#include "test.h"

enum SwitchType {
    SwitchType_unknown,

    SwitchType_silent,
    SwitchType_log_errors,
    SwitchType_run_tests,
    SwitchType_print_help,
    SwitchType_source_file,

    SwitchType_count,
};

internal SwitchType get_switch_type(Char *str) {
    SwitchType res = SwitchType_unknown;

    Int len = string_length(str);
    if(len >= 2) {
        if(str[0] == '-') {
            switch(str[1]) {
                case 'e': res = SwitchType_log_errors;         break;
                case 'h': res = SwitchType_print_help;         break;
#if INTERNAL
                case 's': res = SwitchType_silent;    break;
                case 't': res = SwitchType_run_tests; break;
#endif
                default: assert(0); break;
            }
        } else if((str[len - 1] == 'h') && (str[len - 2] == '.')) {
            res = SwitchType_source_file;
        } else if((str[len - 1] == 'c') && (str[len - 2] == '.')) {
            res = SwitchType_source_file;
        } else if((str[len - 1] == 'p') && (str[len - 2] == 'p') && (str[len - 3] == 'c') && (str[len - 4] == '.') ) {
            res = SwitchType_source_file;
        }
    }

    return(res);
}

internal Bool write_static_file() {
    Char *file =
        "//\n"
        "// Code shared between generated files.\n"
        "#if !defined(STATIC_GENERATED_H)\n"
        "\n"
        "#include <stdio.h>\n"
        "#include <string.h>\n"
        "#include <assert.h>\n"
        "#include <stdlib.h>\n"
        "#include <stddef.h>\n"
        "// TODO(Jonny): Only include these if their needed?\n"
        "#include <vector>\n"
        "#include <deque>\n"
        "#include <forward_list>\n"
        "#include <list>\n"
        "\n"
        "namespace pp { // PreProcessor\n"
        "\n"
        "typedef void _void;\n"
        "typedef char _char;\n"
        "typedef short _short;\n"
        "typedef int _int;\n"
        "typedef long _long;\n"
        "typedef float _float;\n"
        "typedef double _double;\n"
        "typedef bool _bool;\n"
        "\n"
        "#define PP_COMPILER_MSVC 0\n"
        "#define PP_COMPILER_CLANG 0\n"
        "#define PP_COMPILER_GCC 0\n"
        "\n"
        "#define PP_ENVIRONMENT64 0\n"
        "#define PP_ENVIRONMENT32 0\n"
        "\n"
        "#define PP_OS_WIN32 0\n"
        "#define PP_OS_LINUX 0\n"
        "\n"
        "#if defined(__clang__)\n"
        "    #undef PP_COMPILER_CLANG\n"
        "    #define PP_COMPILER_CLANG 1\n"
        "#elif defined(_MSC_VER)\n"
        "    #undef PP_COMPILER_MSVC\n"
        "    #define PP_COMPILER_MSVC 1\n"
        "#elif (defined(__GNUC__) || defined(__GNUG__)) // This has to be after __clang__, because Clang also defines this.\n"
        "    #undef PP_COMPILER_GCC\n"
        "    #define PP_COMPILER_GCC 1\n"
        "#endif\n"
        "\n"
        "#if defined(__linux__)\n"
        "    #undef PP_OS_LINUX\n"
        "    #define PP_OS_LINUX 1\n"
        "#elif defined(_WIN32)\n"
        "    #undef PP_OS_WIN32\n"
        "    #define PP_OS_WIN32 1\n"
        "#endif\n"
        "\n"
        "#if PP_OS_LINUX\n"
        "    #if (__x86_64__ || __ppc64__)\n"
        "        #undef PP_ENVIRONMENT64\n"
        "        #define PP_ENVIRONMENT64 1\n"
        "    #else\n"
        "        #undef PP_ENVIRONMENT32\n"
        "        #define PP_ENVIRONMENT32 1\n"
        "    #endif\n"
        "#elif OS_WIN32\n"
        "    #if defined(_WIN64)\n"
        "        #undef PP_ENVIRONMENT64\n"
        "        #define PP_ENVIRONMENT64 1\n"
        "    #else\n"
        "        #undef PP_ENVIRONMENT32\n"
        "        #define PP_ENVIRONMENT32 1\n"
        "    #endif\n"
        "#endif\n"
        "\n"
        "template<class T, class U>struct TypeCompare_{ static const bool e = false; };\n"
        "template<class T>struct TypeCompare_<T, T>{ static const bool e = true; };\n"
        "#define type_compare(a, b) TypeCompare_<a, b>::e\n"
        "\n"
        "#if defined(_MSC_VER)\n"
        "    #define pp_sprintf(buf, size, format, ...) sprintf_s(buf, size, format, ##__VA_ARGS__)\n"
        "#else\n"
        "    #define pp_sprintf(buf, size, format, ...) sprintf(buf, format, ##__VA_ARGS__)\n"
        "#endif\n"
        "\n"
        "} // namespace pp\n"
        "\n"
        "#define STATIC_GENERATED\n"
        "#endif // !defined(STATIC_GENERATED_H)\n"
        "\n";

    Int static_file_len = string_length(file);
    Bool res = system_write_to_file(dir_name "/static_generated.h", file, static_file_len);

    if(!res) {
        push_error(ErrorType_could_not_write_to_disk);
    }

    return(res);
}

internal Bool should_write_to_file = false;

internal Void start_parsing(Char *fname, Char *file) {
    ParseResult parse_res = parse_stream(file);

    File file_to_write = write_data(fname, parse_res.struct_data, parse_res.struct_cnt,
                                    parse_res.enum_data, parse_res.enum_cnt,
                                    parse_res.func_data, parse_res.func_cnt);

    if(should_write_to_file) {
        Char generated_file_name[256] = dir_name "/"; // TODO(Jonny): MAX_PATH?
        Int start = string_length(dir_name "/");
        Char *generated_extension = "_generated.h";

        // Add _generated.h to the filename.
        if(string_concat(generated_file_name + start, array_count(generated_file_name) - start,
                         fname, string_length(fname) - 4, // TODO(Jonny): Hacky, actually detect the extension properly.
                         generated_extension, string_length(generated_extension))) {
            Bool header_write_success = system_write_to_file(generated_file_name, file_to_write.data, file_to_write.size);
            if(!header_write_success) {
                push_error(ErrorType_could_not_write_to_disk);
            }

            system_free(file_to_write.data);
        }
    }

    for(Int i = 0; (i < parse_res.struct_cnt); ++i) {
        system_free(parse_res.struct_data[i].members);
        system_free(parse_res.struct_data[i].inherited);
    }
    system_free(parse_res.struct_data);

    for(Int i = 0; (i < parse_res.enum_cnt); ++i) {
        system_free(parse_res.enum_data[i].values);
    }
    system_free(parse_res.enum_data);

    for(Int i = 0; (i < parse_res.func_cnt); ++i) {
        system_free(parse_res.func_data[i].params);
    }
    system_free(parse_res.func_data);
}

internal Void print_help(void) {
    Char *help = "    List of Commands.\n"
                 "        -e - Print errors to the console.\n"
                 "        -h - Print this help.\n"
#if INTERNAL
                 "    Internal Commands.\n"
                 "        -s - Do not output any code, just see if there were errors parsing a file.\n"
                 "        -t - Run tests on the code.\n"
#endif
                 "\n";

    system_write_to_console(help);
}

Int main(Int argc, Char **argv) {// TODO(Jonny): Support wildcards.
    Int res = 0;

    system_write_to_console("Starting Mirror...");

    Bool should_log_errors = true;
    if(argc <= 1) {
        push_error(ErrorType_no_parameters);
        print_help();
    } else {
        Bool should_run_tests = false;
        should_write_to_file = true;

        PtrSize largest_source_file_size = 0;
        Int number_of_files = 0;
        for(Int i = 1; (i < argc); ++i) {
            Char *switch_name = argv[i];

            SwitchType type = get_switch_type(switch_name);
            switch(type) {
                case SwitchType_silent:     should_write_to_file = false; break;
                case SwitchType_log_errors: should_log_errors = true;     break;
                case SwitchType_run_tests:  should_run_tests = true;      break;
                case SwitchType_print_help: print_help();                 break;

                case SwitchType_source_file: {
                    if(!string_contains(switch_name, dir_name)) {
                        PtrSize file_size = system_get_file_size(switch_name);
                        if(!file_size) {
                            push_error(ErrorType_cannot_find_file);
                        } else {
                            ++number_of_files;

                            if(file_size > largest_source_file_size) {
                                largest_source_file_size = file_size + 1; // We read the nul-terminator, so this has to be one greater.
                            }
                        }
                    }
                } break;
            }
        }

        if(should_run_tests) {
            Int tests_failed = run_tests();

            if(tests_failed == 0) system_write_to_console("all tests passed...");
            else                  system_write_to_console("%d tests failed\n", tests_failed);
        } else {
            if(!number_of_files) {
                push_error(ErrorType_no_files_pass_in);
            } else {
                Byte *file_memory = system_alloc(Byte, largest_source_file_size);
                if(file_memory) {
                    // Write static file to disk.
                    if(should_write_to_file) {
                        Bool create_folder_success = system_create_folder(dir_name);

                        if(create_folder_success) write_static_file();
                        else                      push_error(ErrorType_could_not_create_directory);
                    }

                    // Parse files
                    for(Int i = 1; (i < argc); ++i) {
                        Char *file_name = argv[i];
                        zero(file_memory, largest_source_file_size);

                        SwitchType type = get_switch_type(file_name);
                        if(type == SwitchType_source_file) {
                            File file = system_read_entire_file_and_null_terminate(file_name, file_memory);

                            if(file.data) start_parsing(file_name, file.data);
                            else          push_error(ErrorType_could_not_load_file);
                        }
                    }

                    system_free(file_memory);
                }
            }
        }

    }

    system_write_to_console("Done");

    if(should_log_errors) {
        if(print_errors()) {
            res = 255;
        }
    }

    system_write_to_console("\n");

    return(res);
}

