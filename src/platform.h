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

#if !defined(_PLATFORM_H)

// Memory.
Void *system_malloc(Uintptr size);
Bool system_free(Void *ptr);
Void *system_realloc(Void *ptr, Uintptr size);
Uintptr system_get_alloc_size(Void *ptr);

// File IO.
struct File {
    Char *e;
    Uintptr size;
};

Bool system_write_to_file(Char *fname, File file);
File system_read_entire_file_and_null_terminate(Char *fname);

Uintptr system_get_total_size_of_directory(Char *dir_name);
Uintptr get_current_directory(Char *buffer, Uintptr size);


// Utility stuff.
Void system_write_to_console(Char *str, ...);

#define _PLATFORM_H
#endif
