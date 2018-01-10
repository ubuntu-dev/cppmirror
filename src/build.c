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

// 3rd party.
#define STB_SPRINTF_IMPLEMENTATION
#define STB_SPRINTF_NOFLOAT
#include "stb_sprintf.h"

// Header files.
#include "types.h"
#include "platform.h"

// Source files.
#include "utilities.cpp"
#include "lexer.cpp"
#include "test.cpp"
#include "write_file.cpp"

#include "main.cpp"

// Windows.
#if OS_WIN32
#include "windows.h"

#include "platform_win32.cpp"

// Linux
#elif OS_LINUX
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

#include "platform_linux.cpp"
#endif
