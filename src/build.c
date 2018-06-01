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

// 3rd party.
#define STB_SPRINTF_IMPLEMENTATION
#define STB_SPRINTF_NOFLOAT
#include "../shared/stb_sprintf.h"

// Header files.
#include "types.h"
#include "platform.h"

// Source files.
#include "utilities.c"
#include "lexer.c"
#include "test.c"
#include "write_file.c"

#include "main.c"

// Windows.
#if OS_WIN32
#include "windows.h"

#include "platform_win32.c"

// Linux
#elif OS_LINUX
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

#include "platform_linux.c"
#endif
