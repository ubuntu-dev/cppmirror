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
    - In serialize struct, if there is a member which is an enum, call enum_to_string on it's value.
    - Allow mathematical macros (1 + 1) to be the index for an array.
    - Global consts for arrays.
    - Support modifiers (unnsigned, volatile, const).
*/

#include "types.h"
#include "utilities.h"
#include "platform.h"
#include "test.h"
#include "lexer.h"
#include "write_file.h"

typedef enum {
    SwitchType_unknown,

    SwitchType_silent,
    SwitchType_log_errors,
    SwitchType_log_errors_as_c,
    SwitchType_run_tests,
    SwitchType_print_help,
    SwitchType_source_file,

    SwitchType_count
} SwitchType;

SwitchType get_switch_type(Char *str) {
    SwitchType res = {0};

    Int len = string_length(str);
    if(len >= 2) {
        if(str[0] == '-') {
            switch(str[1]) {
                case 'e': { res = SwitchType_log_errors;      } break;
                case 'h': { res = SwitchType_print_help;      } break;
#if INTERNAL
                case 's': { res = SwitchType_silent;          } break;
                case 't': { res = SwitchType_run_tests;       } break;
                case 'c': { res = SwitchType_log_errors_as_c; } break;
#endif
                default: assert(0); break;
            }
        } else {
            res = SwitchType_source_file;
        }
    }

    return(res);
}

Void print_help(void) {
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

    Bool is_c = false;
    Bool should_log_errors = true;
    if(argc <= 1) {
        push_error(ErrorType_no_parameters);
        print_help();
    } else {
        Bool should_run_tests = false;
        Bool should_write_to_file = true;

        Int fnames_max_cnt = 16;
        Char **fnames = system_malloc(sizeof(*fnames) * fnames_max_cnt);
        if(fnames) {
            Uintptr total_file_size = 0;
            Int number_of_files = 0;
            for(Int i = 1; (i < argc); ++i) {
                Char *switch_name = argv[i];

                SwitchType type = get_switch_type(switch_name);
                switch(type) {
                    case SwitchType_silent:          { should_write_to_file = false; } break;
                    case SwitchType_log_errors:      { should_log_errors = true;     } break;
                    case SwitchType_run_tests:       { should_run_tests = true;      } break;
                    case SwitchType_print_help:      { print_help();                 } break;
                    case SwitchType_log_errors_as_c: { is_c = true;                  } break;

                    case SwitchType_source_file: {
                        if(number_of_files >= fnames_max_cnt - 1) {
                            fnames_max_cnt *= 2;
                            Void *p = system_realloc(fnames, sizeof(*fnames) * fnames_max_cnt);
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

                if(!tests_failed) { system_write_to_console("all tests passed...");             }
                else              { system_write_to_console("%d tests failed\n", tests_failed); }
            } else {
                if(!number_of_files) {
                    push_error(ErrorType_no_files_pass_in);
                } else {
                    // TODO(Jonny): Remove this!

                    ParseResult parse_res = parse_streams(number_of_files, fnames);
                    File file_to_write = write_data(parse_res, !is_c);
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
