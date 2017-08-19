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
#if 0
extern "C" {
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
    void *memcpy(void *dest, void const *src, size_t count) {
        Byte *dst8 = (Byte *)dest;
        Byte *src8 = (Byte *)src;
        while (count--) {
            *dst8++ = *src8++;
        }

        return(dest);
    }
}
#endif
Uint64 system_get_performance_counter(void) {
    Uint64 res = 0;

    LARGE_INTEGER large_int;
    if(QueryPerformanceCounter(&large_int)) {
        res = large_int.QuadPart;
    }

    return(res);
}

//#define PAGE_ALIGNED_MALLOC
#if defined(PAGE_ALIGNED_MALLOC)
#if 0
struct Allocation_Node {
    Void *block;
    Void *ptr;
    Uintptr size;

    Allocation_Node *next;
};
static Allocation_Node *global_debug_alloc_storage;

Void add_to_global_alloc(Void *block, Void *ptr, Uintptr size) {
    if(!global_debug_alloc_storage) {
        global_debug_alloc_storage = (Allocation_Node *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(*global_debug_alloc_storage));

        global_debug_alloc_storage->block = block;
        global_debug_alloc_storage->ptr = ptr;
        global_debug_alloc_storage->size = size;
    }
    else {
        Allocation_Node *next = global_debug_alloc_storage;
        while(next->next) {
            next = next->next;
        }

        next->next = (Allocation_Node *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(*next->next));
        next = next->next;

        next->block = block;
        next->ptr = ptr;
        next->size = size;
    }
}


Allocation_Node *get_node(Void *ptr) {
    Allocation_Node *res = 0, *next = global_debug_alloc_storage;
    if(next) {
        while(next) {
            if(next->ptr == ptr) {
                res = next;
                break;
            }

            next = next->next;
        }
    }

    return(res);
}

Uintptr get_next_page_boundary(Uintptr size) {
    SYSTEM_INFO si = {0};
    GetSystemInfo(&si);

    Uintptr res = (size / si.dwPageSize) + 1;
    res *= si.dwPageSize;

    assert(res);

    return(res);
}

Void *system_malloc(Uintptr size) {
    Void *res = 0;
    Uintptr alloc_size = get_next_page_boundary(size);

    Void *block = VirtualAlloc(0, alloc_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    assert(block);
    if(block) {
        res = (Byte *)block + (alloc_size - size);
        add_to_global_alloc(block, res, size);
    }
    else {
        Uint err = GetLastError();
        LPSTR msg_buf = 0;
        Uintptr msg_size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                          0, err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&msg_buf, 0, 0);
    }

    return(res);
}

Bool system_free(Void *ptr) {
    Bool res = false;
    if(ptr) {
        Allocation_Node *node = get_node(ptr);
        assert(node);

        node->ptr = 0;
        node->block = 0;
        node->size = 0;
        //res = VirtualFree(ptr, 0, MEM_RELEASE);
        // TODO(Jonny): Could find block in LL then unlink it?
    }

    return(res);
}

Void *system_realloc(Void *ptr, Uintptr size) {
    Void *res = 0;
    if(ptr) {
        Allocation_Node *node = get_node(ptr);
        assert(node);
        if(size > node->size) {
            res = system_malloc(size);
            assert(res);
            if(res) {
                copy(res, node->ptr, node->size);
                system_free(node->ptr);

                // TODO(Jonny): Remove not from the chain.
                node->ptr = 0;
                node->block = 0;
                node->size = 0;
            }
        }
        else {
            assert(0);
            res = ptr;
            node->size = size;
        }
    }
    else {
        res = system_malloc(size);
    }

    return(res);
}
#endif
#else
#if 0
Void *operator new(Uintptr size) {
    Uintptr *res = (Uintptr *)system_malloc(size + sizeof(Uintptr));
    *res++ = size;

    return(res);
}

Void *operator new[](Uintptr size) {
    Uintptr *res = (Uintptr *)system_malloc(size + sizeof(Uintptr));
    *res++ = size;

    return(res);
}

// These won't actually throw, but Clang won't shut up about them...
void operator delete(void *ptr) throw() {
    Uintptr *raw = (Uintptr *)ptr - 1;

    system_free(raw);
}

void operator delete[](void *ptr) throw() {
    Uintptr *raw = (Uintptr *)ptr - 1;

    system_free(raw);
}
#endif

Void *system_malloc(Uintptr size) {
    Uintptr *res = (Uintptr *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size + sizeof(Uintptr));
    *res++ = size;

    return(res);
}

Bool system_free(Void *ptr) {
    Bool res = false;
    if(ptr) {
        Void *raw = (Uintptr *)ptr - 1;
        res = HeapFree(GetProcessHeap(), 0, raw);
    }

    return(res);
}

Void *system_realloc(Void *ptr, Uintptr size) {
    Void *res = 0;
    if(ptr) {
        Void *original_raw = (Uintptr *)ptr - 1;
        Uintptr *new_raw = (Uintptr *)HeapReAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, original_raw, size + sizeof(Uintptr));
        *new_raw++ = size;
    }
    else {
        res = system_malloc(size);
    }

    return(res);
}

Uintptr system_get_alloc_size(Void *ptr) {
    Uintptr res = 0;
    assert(ptr);
    if(ptr) {
        res = *((Uintptr *)ptr - 1);
    }

    return(res);
}

#endif

File system_read_entire_file_and_null_terminate(Char *fname) {
    File res = {0};

    HANDLE fhandle = CreateFileA(fname, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
    if(fhandle != INVALID_HANDLE_VALUE) {
        defer {
            CloseHandle(fhandle);
        };
        LARGE_INTEGER fsize;
        if(GetFileSizeEx(fhandle, &fsize)) {
            DWORD fsize32 = safe_truncate_size_64(fsize.QuadPart);
            Void *memory = system_malloc(fsize32 + 1);

            DWORD bytes_read = 0;
            if(ReadFile(fhandle, memory, fsize32, &bytes_read, 0)) {
                if(bytes_read != fsize32) {
                    push_error(ErrorType_did_not_read_entire_file);
                }
                else {
                    res.size = fsize32;
                    res.e = (Char *)memory;
                    res.e[res.size] = 0;
                }
            }
        }
    }

    return(res);
}

Bool system_write_to_file(Char *fname, File file) {
    Bool res = false;

    HANDLE fhandle = CreateFileA(fname, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, CREATE_ALWAYS, 0, 0);
    if(fhandle != INVALID_HANDLE_VALUE) {
        defer {
            CloseHandle(fhandle);
        };
        DWORD fsize32;
#if ENVIRONMENT32
        fsize32 = file.size;
#else
        fsize32 = safe_truncate_size_64(file.size);
#endif
        DWORD bytes_written = 0;
        if(WriteFile(fhandle, file.e, fsize32, &bytes_written, 0)) {
            if(bytes_written != fsize32) {
                push_error(ErrorType_did_not_write_entire_file);
            }
            else {
                res = true;
            }
        }
    }

    return res;
}

Uintptr system_get_file_size(Char *fname) {
    Uintptr res = 0;

    HANDLE fhandle = CreateFileA(fname, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
    if(fhandle != INVALID_HANDLE_VALUE) {
        defer {
            CloseHandle(fhandle);
        };
        LARGE_INTEGER large_int = {};
        if(GetFileSizeEx(fhandle, &large_int)) {
#if ENVIRONMENT32
            res = safe_truncate_size_64(large_int.QuadPart);
#else
            res = large_int.QuadPart;
#endif
        }

    }

    return(res);
}

Bool is_valid_cpp_file(Char *fname) {
    Bool res = false;

    Int len = string_length(fname);
    if((fname[len - 1] == 'p') && (fname[len - 2] == 'p') && (fname[len - 3] == 'c') && (fname[len - 4] == '.')) {
        res = true;
    }
    else if((fname[len - 1] == 'c') && (fname[len - 2] == 'c') && (fname[len - 3] == '.')) {
        res = true;
    }
    else if((fname[len - 1] == 'c') && (fname[len - 2] == '.')) {
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
    Char *buf = new Char[alloc_size];
    defer {
        delete[] buf;
    };
    if(buf) {
        va_list args;
        va_start(args, format);
        Int sprintf_written = stbsp_vsnprintf(buf, (Int)alloc_size, format, args);
        assert(sprintf_written < alloc_size);
        va_end(args);

        Int len = string_length(buf);
        DWORD chars_written = 0;
        Bool res = WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), buf, len, &chars_written, 0);

        assert(res);
        assert(chars_written == len);
    }
}

int main(int argc_, char **argv_) {
    // Get the command line arguments.
    Char *cmdline = GetCommandLineA();
    Int args_len = string_length(cmdline);

    // Count number of arguments.
    Int original_cnt = 1;
    Bool in_quotes = false;
    for(Int i = 0; (i < args_len); ++i) {
        if(cmdline[i] == '"') {
            in_quotes = !in_quotes;
        }
        else if(cmdline[i] == ' ') {
            if(!in_quotes) {
                ++original_cnt;
            }
        }
    }

    // Create copy of args.
    Char *arg_cpy = new Char[args_len + 1];
    defer {
        delete[] arg_cpy;
    };
    string_copy(arg_cpy, cmdline);

    for(Int i = 0; (i < args_len); ++i) {
        if(arg_cpy[i] == '"') {
            in_quotes = !in_quotes;
        }
        else if(arg_cpy[i] == ' ') {
            if(!in_quotes) {
                arg_cpy[i] = 0;
            }
        }
    }

    // Setup pointers.
    in_quotes = false;
    Int mem_size = original_cnt * 2;
    Int argc = 1;
    Char **argv = new Char *[mem_size];
    defer {
        delete[] argv;
    };
    Char **cur = argv;
    *cur++ = arg_cpy;
    for(Int i = 0; (i < args_len); ++i) {
        if(!arg_cpy[i]) {
            Char *str = arg_cpy + i + 1;
            if(!string_contains(str, '*')) {
                *cur = str;
                ++cur;
                ++argc;
            }
            else {
                WIN32_FIND_DATA find_data;
                HANDLE fhandle = FindFirstFile(str, &find_data);

                if(fhandle != INVALID_HANDLE_VALUE) {
                    do {
                        if(argc + 1 >= mem_size) {
                            mem_size *= 2;
                            Void *p = system_realloc(argv, sizeof(*argv) * mem_size);
                            if(p) {
                                argv = (Char **)p;
                            }
                        }

                        *cur = find_data.cFileName;
                        ++cur;
                        ++argc;
                    }
                    while(FindNextFile(fhandle, &find_data));
                }
            }
        }
    }

    my_main(argc, argv);

    return(0);
    //ExitProcess(0);
}
