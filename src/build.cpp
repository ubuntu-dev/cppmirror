#include "shared.h"
#include "stb_sprintf.h"
#include "platform.h"
#include "lexer.h"
#include "utils.h"
#include "write_file.h"
#include "test.h"

#include "lexer.cpp"
#include "utils.cpp"
#include "write_file.cpp"
#include "test.cpp"
#include "main.cpp"

#if OS_LINUX
#include "platform_linux.cpp"
#elif OS_WIN32
#include "platform_win32.cpp"
#endif