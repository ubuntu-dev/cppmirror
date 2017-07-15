/*===================================================================================================
  File:                    main.c
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
    - Some of the printf formatters in pp_generated (%llu, %lld, %Id, %zu) probably aren't very cross-compiler friendly. I believe
      Visual Studio 2017 is the first one to support long long types correctly, and I don't think GCC supports llu.
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
    SwitchType_is_c_file,
    SwitchType_run_tests,
    SwitchType_print_help,
    SwitchType_output_preprocessed_file,

    SwitchType_source_file,

    SwitchType_count
} SwitchType;

SwitchType get_switch_type(Char *str) {
    SwitchType res = {0};

    Int len = string_length(str);
    if(len >= 2) {
        if(str[0] == '-') {
            switch(str[1]) {
                case 'e': res = SwitchType_log_errors;               break;
                case 'h': res = SwitchType_print_help;               break;
#if INTERNAL
                case 's': res = SwitchType_silent;                   break;
                case 't': res = SwitchType_run_tests;                break;
                case 'c': res = SwitchType_is_c_file;                break;
                case 'p': res = SwitchType_output_preprocessed_file; break;
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
        Bool only_output_preprocessed_file = false;

        Int fnames_max_cnt = 16;
        Char **fnames = system_malloc(sizeof(*fnames) * fnames_max_cnt);
        if(fnames) {
            Uintptr total_file_size = 0;
            Int number_of_files = 0;
            for(Int i = 1; (i < argc); ++i) {
                Char *switch_name = argv[i];

                SwitchType type = get_switch_type(switch_name);
                switch(type) {
                    case SwitchType_silent:                   should_write_to_file = false;         break;
                    case SwitchType_log_errors:               should_log_errors = true;             break;
                    case SwitchType_run_tests:                should_run_tests = true;              break;
                    case SwitchType_print_help:               print_help();                         break;
                    case SwitchType_is_c_file:                is_c = true;                          break;
                    case SwitchType_output_preprocessed_file: only_output_preprocessed_file = true; break;

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

                if(!tests_failed) {
                    system_write_to_console("all tests passed...");
                } else {
                    system_write_to_console("%d tests failed\n", tests_failed);
                }
            } else {
                if(!number_of_files) {
                    push_error(ErrorType_no_files_pass_in);
                } else {
                    File file_to_write = {0};
                    Char *output_string = 0;
                    Parse_Result parse_res = {0};
                    if(only_output_preprocessed_file) {
                        if(number_of_files == 1) {
                            output_string = "something.c";  // TODO(Jonny): Come up with a better name than "something.c"...
                            file_to_write = system_read_entire_file_and_null_terminate(fnames[0]); // TODO(Jonny): Leak.

                            file_to_write = preprocess_macros(file_to_write);
                        } else {
                            system_write_to_console("Can only write one preprocessed file at a time (soz). You input %d file(s).",
                                                    number_of_files);
                        }
                    } else {
                        output_string = "pp_generated.h";

                        parse_res = parse_streams(number_of_files, fnames);

                        file_to_write = write_data(parse_res, !is_c);
                    }

                    if(output_string) {
                        Bool write_success = system_write_to_file(output_string, file_to_write);
                        assert(write_success);
                    }

                    // These are freed so that they don't show up as false positives when I'm checking for memory leaks. I don't
                    // _really_ care about memory leaks though, so they're only freed in an INTERNAL build.
#if INTERNAL
                    system_free(file_to_write.e);
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
