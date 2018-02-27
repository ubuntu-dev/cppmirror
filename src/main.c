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

/* TODO -
    - Have a command, like PP_IGNORE, which you can put just before including a file. If you put this, mirror will not include
      that file when preprocessing.
    - In serialize struct, if there is a member which is an enum, call enum_to_string on it's value.
    - Allow mathematical macros (1 + 1) to be the index for an array.
    - Global consts for arrays.
    - Support modifiers (unnsigned, volatile, const).
    - Some of the printf formatters in pp_generated (%llu, %lld, %Id, %zu) probably aren't very cross-compiler friendly. I believe
      Visual Studio 2017 is the first one to support long long types correctly, and I don't think GCC supports llu.
    - If I declare an enum in C style then it doesn't work.
*/

typedef enum {
    SwitchType_unknown,

    SwitchType_silent,
    SwitchType_log_errors,
    SwitchType_run_tests,
    SwitchType_macro,
    SwitchType_print_help,
    SwitchType_output_preprocessed_file,

    SwitchType_source_file,

    SwitchType_count
} SwitchType;

SwitchType get_switch_type(Char *str) {
    SwitchType res = SwitchType_unknown;

    Int len = string_length(str);
    if(len >= 2) {
        if(str[0] == '-') {
            switch(str[1]) {
                case 'e': case 'E': res = SwitchType_log_errors;               break;
                case 'h': case 'H': res = SwitchType_print_help;               break;
                case 'p': case 'P': res = SwitchType_output_preprocessed_file; break;
                case 'd': case 'D': res = SwitchType_macro;                    break;
#if INTERNAL
                case 's': case 'S': res = SwitchType_silent;                   break;
                case 't': case 'T': res = SwitchType_run_tests;                break;
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

// TODO - This will fail if the format isn't "-Diden=res";
MacroData parse_passed_in_macro(Char *str) {
    Tokenizer tokenizer = {str};
    Token iden = get_token(&tokenizer);
    eat_token(&tokenizer);
    Token result = get_token(&tokenizer);

    MacroData res = {0};
    res.iden = token_to_string(iden);
    res.res = token_to_string(result);

    return(res);
}

Void my_main(Int argc, Char **argv) {
    system_write_to_console("Starting Mirror...");

    Bool should_log_errors = true;
    if(argc <= 1) {
        push_error(ErrorType_no_parameters);
        print_help();
    } else {
        Bool should_run_tests = false;
        Bool should_write_to_file = true;
        Bool only_output_preprocessed_file = false;

        Int fnames_max_cnt = 16;
        //Char **fnames = new Char *[fnames_max_cnt];
        Char **fnames = system_malloc_arr(sizeof *fnames, fnames_max_cnt);
        Uintptr size = system_get_alloc_size(fnames);
        if(fnames) {
            Uintptr total_file_size = 0;
            Int number_of_files = 0;

            Int macro_cnt = 0;
            MacroData passed_in_macro_data[8] = {0};

            for(Int i = 1; (i < argc); ++i) {
                Char *switch_name = argv[i];

                SwitchType type = get_switch_type(switch_name);
                switch(type) {
                    case SwitchType_silent:                   should_write_to_file = false;         break;
                    case SwitchType_log_errors:               should_log_errors = true;             break;
                    case SwitchType_run_tests:                should_run_tests = true;              break;
                    case SwitchType_print_help:               print_help();                         break;
                    case SwitchType_output_preprocessed_file: only_output_preprocessed_file = true; break;

                    case SwitchType_macro:
                        passed_in_macro_data[macro_cnt++] = parse_passed_in_macro(argv[i] + 2); // Skip "-D".
                        break;

                    case SwitchType_source_file: {
                        if(number_of_files >= fnames_max_cnt - 1) {
                            fnames_max_cnt *= 2;
                            Void *p = system_realloc(fnames, sizeof(*fnames) * fnames_max_cnt);
                            if(p) {
                                fnames = (Char **)p;
                            }
                        }

                        fnames[number_of_files++] = switch_name;
                    } break;
                }
            }

            if(should_run_tests) {
#if INTERNAL
                Int tests_failed = run_tests();

                if(!tests_failed) {
                    system_write_to_console("all tests passed...");
                } else {
                    system_write_to_console("%d tests failed\n", tests_failed);
                }
#endif
            } else {
                if(!number_of_files) {
                    push_error(ErrorType_no_files_pass_in);
                } else {
                    Char directory[1024] = {0};
                    get_current_directory(directory, 1024);
                    Uintptr size_of_all_files = system_get_total_size_of_directory(directory);

                    Parse_Result parse_res = parse_streams(number_of_files, fnames, passed_in_macro_data,
                                                           macro_cnt, size_of_all_files);

                    if(parse_res.success) {
                        File file_to_write = write_data(parse_res);

                        Bool write_success = system_write_to_file("pp_generated.h", file_to_write);
                        assert(write_success);
                        
#if INTERNAL
                        system_free(file_to_write.e);
#endif
                    }

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
        Int err_cnt = print_errors();
        assert(err_cnt == 0);
    }

    system_write_to_console("\n");
}
