/*===================================================================================================
  File:                    platform.h
  Author:                  Jonathan Livingstone
  Email:                   seagull127@ymail.com
  Licence:                 Public Domain
                           No Warranty is offered or implied about the reliability,
                           suitability, or usability
                           The use of this code is at your own risk
                           Anyone can use this code, modify it, sell it to terrorists, etc.
  ===================================================================================================*/

// Memory.
#define system_alloc(Type, ...) (Type *)system_malloc(sizeof(Type), ##__VA_ARGS__)
Void *system_malloc(Uintptr size, Uintptr cnt = 1);
Bool system_free(Void *ptr);
Void *system_realloc(Void *ptr, Uintptr size);

// File IO.
struct File {
    Char *e;
    Uintptr size;
};

Bool system_write_to_file(Char *fname, File file);
File system_read_entire_file_and_null_terminate(Char *fname);

// Utility stuff.
Void system_write_to_console(Char *str, ...);
