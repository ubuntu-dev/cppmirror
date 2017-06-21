/*===================================================================================================
  File:                    build.c
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

// Headers
#include "types.h"
#include "platform.h"

// C++ Files.
#include "utils.c"
#include "lexer.c"
#include "write_file.c"
#if INTERNAL
    #include "test.c"
#endif

// Platform-specific implementations.
#if OS_LINUX
    #include <stdio.h>    // File IO.
    #include <sys/stat.h> // mkdir

    #include "platform_linux.c"

#elif OS_WIN32
    #include <windows.h>

    #include "platform_win32.c"
#endif

// main.
#include "main.c"
