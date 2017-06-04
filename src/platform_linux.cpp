/*===================================================================================================
  File:                    platform_linux.cpp
  Author:                  Jonathan Livingstone
  Email:                   seagull127@ymail.com
  Licence:                 Public Domain
                           No Warranty is offered or implied about the reliability,
                           suitability, or usability
                           The use of this code is at your own risk
                           Anyone can use this code, modify it, sell it to terrorists, etc.
  ===================================================================================================*/

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "platform.h"
#include "utils.h"
#include "stb_sprintf.h"

Void *system_malloc(Uintptr size, Uintptr cnt/*= 1*/) {
    Void *res = malloc(size * cnt);
    if(res) {
        zero(res, size * cnt);
    }

    return(res);
}

Bool system_free(Void *ptr) {
    Bool res = false;
    if(ptr) {
        free(ptr);
        res = true;
    }

    return(res);
}

Void *system_realloc(Void *ptr, Uintptr new_size) {
    Void *res = realloc(ptr, new_size);
    // TODO(Jonny): Is there a realloc and zero for linux?

    return(res);
}

File system_read_entire_file_and_null_terminate(Char *fname) {
    File res = {};

    FILE *file = fopen(fname, "r");
    if(file) {
        fseek(file, 0, SEEK_END);
        res.size = ftell(file);
        fseek(file, 0, SEEK_SET);

        res.e = system_alloc(Char, res.size + 1);
        fread(res.e, 1, res.size, file);
        res.e[res.size] = 0;
        fclose(file);
    }

    return(res);
}

Bool system_write_to_file(Char *fname, File file) {
    assert(file.size > 0);

    Bool res = false;

    FILE *fhandle = fopen(fname, "w");
    if(fhandle) {
        fwrite(file.e, 1, file.size, fhandle);
        fclose(fhandle);
        res = true;
    }

    return(res);
}

Bool system_create_folder(Char *name) {
    Bool res = false;
    struct stat st = {};

    if(stat(name, &st) == -1) {
        res = (mkdir(name, 0700) == 0);
    } else {
        res = true;
    }

    return(res);
}

Void system_write_to_console(Char *format, ...) {
    Uintptr alloc_size = 1024;
    Char *buf = system_alloc(Char, alloc_size);
    if(buf) {
        va_list args;
        va_start(args, format);
        Int sprintf_written = stbsp_vsnprintf(buf, alloc_size, format, args);
        assert(sprintf_written < alloc_size);
        va_end(args);

        printf("%s", buf);

        system_free(buf);
    }

}
