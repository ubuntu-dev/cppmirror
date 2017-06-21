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
Void *system_malloc(Uintptr size);
Bool system_free(Void *ptr);
Void *system_realloc(Void *ptr, Uintptr size);

// File IO.
typedef struct {
    Char *e;
    Uintptr size;
} File;

Bool system_write_to_file(Char *fname, File file);
File system_read_entire_file_and_null_terminate(Char *fname);

// Utility stuff.
Void system_write_to_console(Char *str, ...);
