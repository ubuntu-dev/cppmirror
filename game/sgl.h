/*
    sgl - v0.1 - public domain helper library - no warranty implied; use at your own risk

    Write #define SGL_IMPLEMENTATION in ONE of the C/C++ files to create the implementation.

    // It should look like this.
    #define SGL_IMPLEMENTATION
    #include "sgl.h"

    LICENSE at end of file.
*/

#if !defined(_SGL_H)

#if defined(__cplusplus) && !defined(SGL_NO_EXTERN_C)
    #define SGL_EXTERN_C_BEGIN extern "C" {
    SGL_EXTERN_C_BEGIN
#endif

//
// Compiler/platform macros.
//
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

//
// Export macros
//
#define SGL_EXPORT
#define SGL_IMPORT
// MSVC
#if SGL_COMPILER_MSVC
    #if defined(SGL_EXPORT)
        #undef SGL_EXPORT
    #endif
    #define SGL_EXPORT __declspec(dllexport)

    #if defined(SGL_IMPORT)
        #undef SGL_IMPORT
    #endif
    #define SGL_IMPORT __declspec(dllimport)
    // GCC/Clang
#elif SGL_COMPILER_GCC || SGL_COMPILER_CLANG
    #if defined(SGL_EXPORT)
        #undef SGL_EXPORT
    #endif

    #define SGL_EXPORT __attribute__((visibility("default")))
#endif

//
// Types
//
#include <stdint.h>

typedef uint64_t sgl_Uint64;
typedef uint32_t sgl_Uint32;
typedef uint16_t sgl_Uint16;
typedef uint8_t sgl_Uint8;

typedef int64_t sgl_Int64;
typedef int32_t sgl_Int32;
typedef int16_t sgl_Int16;
typedef int8_t sgl_Int8;

typedef sgl_Int32 sgl_Int;
typedef sgl_Uint32 sgl_Uint;

typedef sgl_Uint8 sgl_Byte;
typedef uintptr_t sgl_Uintptr;
typedef intptr_t sgl_Intptr;

typedef float sgl_Float32;
typedef double sgl_Float64;
typedef sgl_Float32 sgl_Float;

typedef void sgl_Void;
typedef char sgl_Char;
typedef int sgl_Bool;

#define SGL_TRUE 1
#define SGL_FALSE 0

// Use #define SGL_NO_BOOL to avoid create Bool typedef here.
#if !defined(SGL_NO_TYPES)
    typedef sgl_Uint64 Uint64;
    typedef sgl_Uint32 Uint32;
    typedef sgl_Uint16 Uint16;
    typedef sgl_Uint8 Uint8;
    typedef sgl_Int64 Int64;
    typedef sgl_Int32 Int32;
    typedef sgl_Int16 Int16;
    typedef sgl_Int8 Int8;
    typedef sgl_Int Int;
    typedef sgl_Uint Uint;
    typedef sgl_Byte Byte;
    typedef sgl_Uintptr Uintptr;
    typedef sgl_Intptr Intptr;
    typedef sgl_Float32 Float32;
    typedef sgl_Float64 Float64;
    typedef sgl_Float Float;
    typedef sgl_Void Void;
    typedef sgl_Char Char;

    #if !defined(SGL_NO_BOOL)
        #if defined(Bool)
            #undef Bool
        #endif
        typedef sgl_Bool Bool;
    #endif
    #define true SGL_TRUE
    #define false SGL_FALSE
#endif //!defined(SGL_NO_TYPES)

//
// Macros
//
#define SGL_ARRAY_COUNT(arr) (sizeof(arr) / (sizeof(*(arr))))
#define SGL_PREPROCESSOR_CONCAT(a, b) a##b

// A quick and useful assert
#if SGL_INTERNAL
    #define SGL_ASSERT(exp) do { static int ignore = 0; if(!ignore) { if(!(exp)) {*(Uintptr volatile *)0 = 0; } } } while(0)
#else
    #define SGL_ASSERT(exp) {}
#endif

//
// Functions
//

//
// Utils
#define sgl_zero(dest, size) sgl_memset(dest, 0, size)
void sgl_memset(void *dest, uint8_t x, uintptr_t size);
void sgl_memcpy(void *dest, const void *src, uintptr_t num);

//
// String
int sgl_string_len(char const *str);

#define sgl_string_comp(a, b) sgl_string_comp_len(a, b, 0)
sgl_Bool sgl_string_comp_len(char const *a, char const *b, uintptr_t len);
sgl_Bool sgl_is_letter(char c);
sgl_Bool sgl_is_number(char c);
sgl_Bool sgl_is_lower(char c);
sgl_Bool sgl_is_upper(char c);
sgl_Bool sgl_to_lower(char c);
sgl_Bool sgl_to_upper(char c);

#define SGL_KILOBYTES(v) ((v)                * (1024LL))
#define SGL_MEGABYTES(v) ((SGL_KILOBYTES(v)) * (1024LL))
#define SGL_GIGABYTES(v) ((SGL_MEGABYTES(v)) * (1024LL))

//
// If no crt
//
#define SGL_NO_CRT 0

#if defined(SGL_NO_CRT_WINDOW_APP) || defined(SGL_NO_CRT_CONSOLE_APP) || defined(SGL_NO_CRT_DLL)
    #undef SGL_NO_CRT
    #define SGL_NO_CRT 1
#endif // #if defined(SGL_NO_CRT_WINDOW_APP) || defined(SGL_NO_CRT_CONSOLE_APP)


#if SGL_COMPILER_MSVC && SGL_NO_CRT

    #pragma function(memset)
    void *__cdecl memset(void *dest, int c, size_t count);

    #pragma function(memcpy)
    void *__cdecl memcpy(void *dest, void const *src, size_t count);

#endif // SGL_COMPILER_MSVC && SGL_NO_CRT

#if defined(SGL_IMPLEMENTATION)

//
// malloc/memset
//
void sgl_memset(void *dest, uint8_t x, uintptr_t size) {
    uint8_t *dest8 = (uint8_t *)dest;
    for (uintptr_t i = 0; (i < size); ++i) {
        dest8[i] = x;
    }
}

void sgl_memcpy(void *dest, const void *src, uintptr_t num) {
    uint8_t *dest8 = (uint8_t *)dest;
    uint8_t *src8 = (uint8_t *)src;
    for (uintptr_t i = 0; (i < num); ++i) {
        *dest8++ = *src8++;
    }
}

//
// String
//
int sgl_string_len(char const *str) {
    int res = 0;
    if(str) {
        while(str[res++]);
    }

    return(res);
}

sgl_Bool sgl_string_comp_len(char const *a, char const *b, uintptr_t len) {
    sgl_Bool res = true;

    if(len == 0) {
        int a_len = sgl_string_len(a);
        int b_len = sgl_string_len(b);
        if (a_len != b_len) {
            res = false;
        } else {
            len = a_len;
        }
    }

    if(res) {
        for (uintptr_t i = 0; (i < len); ++i, ++a, ++b) {
            if (*a != *b) {
                res = false;
                break;
            }
        }
    }

    return (res);
}

sgl_Bool sgl_is_letter(char c) {
    if((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) {
        return(SGL_TRUE);
    } else {
        return(SGL_FALSE);
    }
}

sgl_Bool sgl_is_number(char c) {
    if(c >= '0' && c <= '9') {
        return(SGL_TRUE);
    } else {
        return(SGL_FALSE);
    }
}

sgl_Bool sgl_is_lower(char c) {
    if(c >= 'a' && c <= 'z') {
        return(SGL_TRUE);
    } else {
        return(SGL_FALSE);
    }
}

sgl_Bool sgl_is_upper(char c) {
    if(c >= 'A' && c <= 'Z') {
        return(SGL_TRUE);
    } else {
        return(SGL_FALSE);
    }
}

sgl_Bool sgl_to_lower(char c) {
    if(sgl_is_upper(c)) {
        c += ('a' - 'A');
    }

    return (c);
}

sgl_Bool sgl_to_upper(char c) {
    if(sgl_is_lower(c)) {
        c -= ('a' - 'A');
    }

    return (c);
}

//
// If no crt
//
#if SGL_COMPILER_MSVC && SGL_NO_CRT
// MSVC need to see "_fltused" before you can use floats (normally defined in the CRT source). No idea why, or why
// it wants the value "0x9875".
int _fltused = 0x9875;

#pragma function(memset)
void *__cdecl memset(void *dest, int c, size_t count) {
    SGL_ASSERT(c > 0 && c < 0xFF);

    uint8_t val = *(uint8_t *)&c; // Done a little weird to try and avoid sign extending/diminishing.
    uint8_t *dest8 = (uint8_t *)dest;
    while (count--) {
        *dest8++ = val;
    }

    return (dest);
}

#pragma function(memcpy)
void *__cdecl memcpy(void *dest, void const *src, size_t count) {
    uint8_t *dst8 = (uint8_t *)dest;
    uint8_t *src8 = (uint8_t *)src;
    while (count--) {
        *dst8++ = *src8++;
    }

    return (dest);
}

// This function is used to probe the stack and expand the 4k (or 8k in x64) size. However, I don't think it's possible to
// implement in C and MSVC x64 compiler doesn't support inline asm. So just assert false if we ever call it.
// Should never be required if you compile with the flags "-GS- -Gs9999999".
void __chkstk(void) {SGL_ASSERT(0);}

// These functions do some stack checking and other nonsense to prevent "hackers" (because Windows never has those?)
// Just define them as stubs.
void __report_rangecheckfailure(void) {}
void __GSHandlerCheck(void) {}
void __security_check_cookie(uintptr_t foo) {}
uintptr_t __security_cookie;

#include <windows.h> // TODO - Get rid of this

// Windows doesn't really let you mix console and window apps. So need to have the user define which one they're using.
#if defined(SGL_NO_CRT_WINDOW_APP)
int __stdcall WinMain(HINSTANCE instance, HINSTANCE prev_instance, LPSTR command_line, int show_code);
void __stdcall WinMainCRTStartup(void) {
    // TODO - Pass other values in.
    int result = WinMain(GetModuleHandle(0), 0, 0, 0);
    ExitProcess(result); // TODO - Is it safe to just return from WinMainCRTStartup rather than ExitProcess??
}
#endif // defined(SGL_NO_CRT_WINDOW_APP)

#if defined(SGL_NO_CRT_CONSOLE_APP)
int main(int argc, char **argv);
void __stdcall mainCRTStartup(void) {
    // TODO - Pass other values in.
    int result = main(0, 0);
    ExitProcess(result); // TODO - Is it safe to just return from mainCRTStartup rather than ExitProcess??
}
#endif // defined(SGL_NO_CRT_CONSOLE_APP)

#if defined(SGL_NO_CRT_DLL)
// TODO - I don't think I need to do anything in here?
BOOL __stdcall DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    return (SGL_TRUE);
}
void __stdcall _DllMainCRTStartup(void) {
    DllMain(0, 0, 0);
}
#endif // defined(SGL_NO_CRT_DLL)

#endif


#endif

#if defined(__cplusplus) && !defined(SGL_NO_EXTERN_C)
    #define SGL_EXTERN_C_END }
    SGL_EXTERN_C_BEGIN
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