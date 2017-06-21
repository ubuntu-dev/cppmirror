/*===================================================================================================
  File:                    platform_win32.cpp
  Author:                  Jonathan Livingstone
  Email:                   seagull127@ymail.com
  Licence:                 Public Domain
                           No Warranty is offered or implied about the reliability,
                           suitability, or usability
                           The use of this code is at your own risk
                           Anyone can use this code, modify it, sell it to terrorists, etc.
  ===================================================================================================*/

int _fltused;

#pragma function(memset)
void *memset(void *dest, int c, size_t count) {
    assert(c < 0xFF);
    Byte *dest8 = (Byte *)dest;
    while(count--) {
        *dest8++ = (Byte)c;
    }

    return(dest);
}

#pragma function(memcpy)
void *memcpy(void *dest, const void *src, size_t count) {
    Byte *dst8 = (Byte *)dest;
    Byte *src8 = (Byte *)src;
    while (count--) {
        *dst8++ = *src8++;
    }

    return(dest);
}

Uint64 system_get_performance_counter() {
    Uint64 res = 0;

    LARGE_INTEGER large_int;
    if(QueryPerformanceCounter(&large_int)) {
        res = large_int.QuadPart;
    }

    return(res);
}

// TODO(Jonny): Replace these with VirtualAlloc? Then I could make sure they all end of a page boundary,
//              and I'd know I wasn't overwritting them. Realloc could be implemented as a VirtualAlloc/copy.
Void *system_malloc(Uintptr size) {
    Void *res = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size);

    return(res);
}

Bool system_free(Void *ptr) {
    Bool res = false;
    if(ptr) {
        res = HeapFree(GetProcessHeap(), 0, ptr) != 0;
    }

    return(res);
}

Void *system_realloc(Void *ptr, Uintptr size) {
    Void *res = 0;
    if(ptr) {
        res = HeapReAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, ptr, size);
    } else {
        res = system_malloc(size);
    }

    return(res);
}

File system_read_entire_file_and_null_terminate(Char *fname) {
    File res = {0};

    HANDLE fhandle = CreateFileA(fname, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
    if(fhandle != INVALID_HANDLE_VALUE) {
        LARGE_INTEGER fsize;
        if(GetFileSizeEx(fhandle, &fsize)) {
            DWORD fsize32 = safe_truncate_size_64(fsize.QuadPart);
            void *memory = system_malloc(fsize32 + 1);

            DWORD bytes_read;
            if(ReadFile(fhandle, memory, fsize32, &bytes_read, 0)) {
                if(bytes_read != fsize32) {
                    push_error(ErrorType_did_not_read_entire_file);
                } else {
                    res.size = fsize32;
                    res.e = cast(Char *)memory;
                    res.e[res.size] = 0;
                }
            }

            CloseHandle(fhandle);
        }
    }

    return(res);
}

Bool system_write_to_file(Char *fname, File file) {
    Bool res = false;

    HANDLE fhandle = CreateFileA(fname, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, CREATE_ALWAYS, 0, 0);
    if(fhandle != INVALID_HANDLE_VALUE) {
        DWORD fsize32;
#if ENVIRONMENT32
        fsize32 = file.size;
#else
        fsize32 = safe_truncate_size_64(file.size);
#endif
        DWORD bytes_written;
        if(WriteFile(fhandle, file.e, fsize32, &bytes_written, 0)) {
            if(bytes_written != fsize32) push_error(ErrorType_did_not_write_entire_file);
            else                         res = true;
        }

        CloseHandle(fhandle);
    }

    return res;
}

Uintptr system_get_file_size(Char *fname) {
    Uintptr res = 0;

    HANDLE fhandle = CreateFileA(fname, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
    if(fhandle != INVALID_HANDLE_VALUE) {
        LARGE_INTEGER large_int;
        if(GetFileSizeEx(fhandle, &large_int)) {
#if ENVIRONMENT32
            res = safe_truncate_size_64(large_int.QuadPart);
#else
            res = large_int.QuadPart;
#endif
        }

        CloseHandle(fhandle);
    }

    return(res);
}

Bool is_valid_cpp_file(Char *fname) {
    Bool res = false;

    Int len = string_length(fname);
    if((fname[len - 1] == 'p') && (fname[len - 2] == 'p') && (fname[len - 3] == 'c') && (fname[len - 4] == '.')) {
        res = true;
    } else if((fname[len - 1] == 'c') && (fname[len - 2] == 'c') && (fname[len - 3] == '.')) {
        res = true;
    } else if((fname[len - 1] == 'c') && (fname[len - 2] == '.')) {
        res = true;
    }

    return(res);
}

Bool system_create_folder(Char *name) {
    Int create_dir_res = CreateDirectory(name, 0);

    Bool res = (create_dir_res == 0);

    return(res);
}

Void system_write_to_console(Char *format, ...) {
    Uintptr alloc_size = 1024;
    Char *buf = system_malloc(sizeof(*buf) * alloc_size);
    if(buf) {
        va_list args;
        va_start(args, format);
        Int sprintf_written = stbsp_vsnprintf(buf, (Int)alloc_size, format, args);
        assert(sprintf_written < alloc_size);
        va_end(args);

        Int len = string_length(buf);
        DWORD chars_written = 0;
        Bool res = WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), buf, len, &chars_written, 0) != 0;

        assert(res);
        assert(chars_written == len);

        system_free(buf);
    }
}

int main(int argc, char **argv);
void mainCRTStartup() {
    // Get the command line arguments.
    Char *cmdline = GetCommandLineA();
    Int args_len = string_length(cmdline);

    // Count number of arguments.
    Int original_cnt = 1;
    Bool in_quotes = false;
    for(Int i = 0; (i < args_len); ++i) {
        if(cmdline[i] == '"') {
            in_quotes = !in_quotes;
        } else if(cmdline[i] == ' ') {
            if(!in_quotes) {
                ++original_cnt;
            }
        }
    }

    // Create copy of args.
    Char *arg_cpy = system_malloc(sizeof(*arg_cpy) * (args_len + 1));
    string_copy(arg_cpy, cmdline);

    for(Int i = 0; (i < args_len); ++i) {
        if(arg_cpy[i] == '"') {
            in_quotes = !in_quotes;
        } else if(arg_cpy[i] == ' ') {
            if(!in_quotes) {
                arg_cpy[i] = 0;
            }
        }
    }

    // Setup pointers.
    in_quotes = false;
    Int mem_size = original_cnt * 2;
    Int argc = 1;
    Char **argv = system_malloc(sizeof(*argv) * mem_size);
    Char **cur = argv;
    *cur = arg_cpy;
    ++cur;
    for(Int i = 0; (i < args_len); ++i) {
        if(!arg_cpy[i]) {
            Char *str = arg_cpy + i + 1;
            if(!cstring_contains(str, '*')) {
                *cur = str;
                ++cur;
                ++argc;
            } else {
                WIN32_FIND_DATA find_data;
                HANDLE fhandle = FindFirstFile(str, &find_data);

                if(fhandle != INVALID_HANDLE_VALUE) {
                    do {
                        if(argc + 1 >= mem_size) {
                            mem_size *= 2;
                            Void *p = system_realloc(argv, sizeof(Char *) * mem_size);
                            if(p) {
                                argv = cast(Char **)p;
                            }
                        }

                        *cur = find_data.cFileName;
                        ++cur;
                        ++argc;
                    } while(FindNextFile(fhandle, &find_data));
                }
            }
        }
    }

    int res = main(argc, argv);

    system_free(arg_cpy);
    system_free(argv);

    ExitProcess(res);
}