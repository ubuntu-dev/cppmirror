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
        } else {
            // If it's not a flag, assume it's a source file. This may not actually be true though...
            res = SwitchType_source_file;
        }
    }

    return(res);
}

internal Bool should_write_to_file = false;

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

int main(int argc, char **argv) {
    Int res = 0;

    system_write_to_console("Starting Mirror...");

    Bool should_log_errors = true;
    if(argc <= 1) {
        push_error(ErrorType_no_parameters);
        print_help();
    } else {
        Bool should_run_tests = false;
        should_write_to_file = true;

        Int fnames_max_cnt = 16;
        Char **fnames = system_alloc(Char *, fnames_max_cnt);
        if(fnames) {
            PtrSize total_file_size = 0;
            Int number_of_files = 0;
            for(Int i = 1; (i < argc); ++i) {
                Char *switch_name = argv[i];

                SwitchType type = get_switch_type(switch_name);
                switch(type) {
                    case SwitchType_silent:     { should_write_to_file = false; } break;
                    case SwitchType_log_errors: { should_log_errors = true;     } break;
                    case SwitchType_run_tests:  { should_run_tests = true;      } break;
                    case SwitchType_print_help: { print_help();                 } break;

                    case SwitchType_source_file: {
                        if(number_of_files >= fnames_max_cnt - 1) {
                            fnames_max_cnt *= 2;
                            Void *p = system_realloc(fnames, fnames_max_cnt * sizeof(Char *));
                            if(p) {
                                fnames = cast(Char **)p;
                            }
                        }

                        fnames[number_of_files++] = switch_name;
                    } break;
                }
            }

            if(should_run_tests) {
                Int tests_failed = run_tests();

                if(tests_failed == 0) { system_write_to_console("all tests passed...");             }
                else                  { system_write_to_console("%d tests failed\n", tests_failed); }
            } else {
                if(!number_of_files) {
                    push_error(ErrorType_no_files_pass_in);
                } else {
                    // TODO(Jonny): Remove this!

                    ParseResult parse_res = parse_streams(number_of_files, fnames);
                    File file_to_write = write_data(parse_res);
                    Bool write_success = system_write_to_file("pp_generated.h", file_to_write);

#if INTERNAL
                    system_free(parse_res.enums.e);
                    system_free(parse_res.structs.e);
                    system_free(parse_res.funcs.e);
                    system_free(parse_res.typedefs.e);
#endif
                }
            }

            system_free(fnames);
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
