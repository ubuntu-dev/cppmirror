/*
    sgl - v0.1 - public domain helper library - no warranty implied; use at your own risk

    Write #define SGL_IMPLEMENTATION in ONE of the C/C++ files to create the implementation.

    // It should look like this.
    #define SGL_IMPLEMENTATION
    #include "sgl.h"

    LICENSE at end of file.
*/

#if !defined(_SGL_H)

#define SGL_COMPILER_MSVC 0
#define SGL_COMPILER_CLANG 0
#define SGL_COMPILER_GCC 0

#define SGL_ENVIRONMENT64 1
#define SGL_ENVIRONMENT32 0

#define SGL_OS_WIN32 0
#define SGL_OS_LINUX 0

#if defined(__clang__)
    #undef SGL_COMPILER_CLANG
    #define SGL_COMPILER_CLANG 1
#elif defined(_MSC_VER)
    #undef SGL_COMPILER_MSVC
    #define SGL_COMPILER_MSVC 1
#elif (defined(__GNUC__) || defined(__GNUG__)) // This has to be after __clang__, because Clang also defines this.
    #undef SGL_COMPILER_GCC
    #define SGL_COMPILER_GCC 1
#else
    #error "Could not detect compiler."
#endif

#if defined(__linux__)
    #undef SGL_OS_LINUX
    #define SGL_OS_LINUX 1
#elif defined(_WIN32)
    #undef SGL_OS_WIN32
    #define SGL_OS_WIN32 1
#else
    #error "Could not detect OS"
#endif

#if SGL_OS_LINUX
    #if (__x86_64__ || __ppc64__)
        #undef SGL_ENVIRONMENT64
        #define SGL_ENVIRONMENT64 1
    #else
        #undef SGL_ENVIRONMENT32
        #define SGL_ENVIRONMENT32 1
    #endif
#elif SGL_OS_WIN32
    #if defined(_WIN64)
        #undef SGL_ENVIRONMENT64
        #define SGL_ENVIRONMENT64 1
    #else
        #undef SGL_ENVIRONMENT32
        #define SGL_ENVIRONMENT32 1
    #endif
#endif

#include <stdint.h>

typedef int sgl_Bool;
#define SGL_TRUE 1
#define SGL_FALSE 0

// Use #define SGL_NO_BOOL to avoid create Bool typedef here.
#if !defined(SGL_NO_BOOL)
    #if defined(Bool)
        #undef Bool
    #endif
    #define Bool sgl_Bool

    #if defined(true)
        #undef true
    #endif

    #define true SGL_TRUE

    #if defined(false)
        #undef false
    #endif
    #define false SGL_FALSE
#endif

void sgl_memset(void *dest, uint8_t x, uintptr_t size);

int sgl_string_len(char const *str);

char sgl_to_upper(char c);

sgl_Bool sgl_is_letter(char c);
sgl_Bool sgl_is_number(char c);

#if defined(SGL_IMPLEMENTATION)

void sgl_memset(void *dest, uint8_t x, uintptr_t size) {
    uint8_t *dest8 = (uint8_t *)dest;
    for(int i = 0; (i < size); ++i) {
        dest8[i] = x;
    }
}

int sgl_string_len(char const *str) {
    int res = 0;
    while(str[res++]);

    return(res);
}

char sgl_to_upper(char c) {
    if((c >= 'a') && (c <= 'z')) {
        c -= ('a' - 'A');
    }

    return(c);
}

sgl_Bool sgl_is_letter(char c) {
    if((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) {
        return SGL_TRUE;
    } else {
        return SGL_FALSE;
    }
}

sgl_Bool sgl_is_number(char c) {
    if(c >= '0' && c <= '9') {
        return SGL_TRUE;
    } else {
        return SGL_FALSE;
    }
}

#endif

#define _SGL_H
#endif

/*
------------------------------------------------------------------------------
This software is available under 2 licenses -- choose whichever you prefer.
------------------------------------------------------------------------------
ALTERNATIVE A - Public Domain (www.unlicense.org)
This is free and unencumbered software released into the public domain.
Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
software, either in source code form or as a compiled binary, for any purpose,
commercial or non-commercial, and by any means.
In jurisdictions that recognize copyright laws, the author or authors of this
software dedicate any and all copyright interest in the software to the public
domain. We make this dedication for the benefit of the public at large and to
the detriment of our heirs and successors. We intend this dedication to be an
overt act of relinquishment in perpetuity of all present and future rights to
this software under copyright law.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------------------------
ALTERNATIVE B - MIT License
Copyright (c) 2017 Jonathan Livingstone
Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
------------------------------------------------------------------------------
*/