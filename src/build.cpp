/*===================================================================================================
  File:                    build.cpp
  Author:                  Jonathan Livingstone
  Email:                   seagull127@ymail.com
  Licence:                 Public Domain
                           No Warranty is offered or implied about the reliability,
                           suitability, or usability
                           The use of this code is at your own risk
                           Anyone can use this code, modify it, sell it to terrorists, etc.
  ===================================================================================================*/

// Standard
#include <stdint.h>
#include <stdarg.h>  // va_list.
#include <stdlib.h>  // va_arg()

// 3rd party
#define STB_SPRINTF_IMPLEMENTATION
#define STB_SPRINTF_NOFLOAT
#include "stb_sprintf.h"

// Anoymous namespaces make every function inside them "static", which help with optimizations, even in a unity build.
namespace {

// Headers
#include "types.h"
#include "platform.h"

// C++ Files.
#include "utils.cpp"
#include "lexer.cpp"
#include "write_file.cpp"
#if INTERNAL
    #include "test.cpp"
#endif

// Platform-specific implementations.
#if OS_LINUX
    #include <stdio.h>    // File IO.
    #include <sys/stat.h> // mkdir

    #include "platform_linux.cpp"

#elif OS_WIN32
    #include <windows.h>

    #include "platform_win32.cpp"
#endif

} // namespace

// main.
#include "main.cpp"
