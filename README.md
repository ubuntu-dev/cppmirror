# Mirror
## Overview

This is a simple project which aims to provide some meta programming functionality not currently avaiable in C++11 through code generation.

All generated code strictly follows the C++11 specification, and should compile correctly under MSVC, Clang, or GCC, even with `-Wall` enabled.

## Build instructions

### Windows - MSVC
Call `win32_msvc.bat` from the command line after previously calling 'vcvarsal.bat' in your favourite version of Visual Studio. This will generate the executable inside the `build` directory.

### Linux - Clang
Call `linux_clang.sh` from the command line, which should generate the app file inside the `build` directory. This uses Clang version 3.8, so if you don't have that version installed on your computer, then you'll need to change the line that says `CLANG_VERSION=3.8` to match the version you've got.

## Contact

Any bugs, suggestions, complaints, or just general feedback, should be emailed to: seagull127@ymail.com.

## LICENSE

This software is dual-licensed to the public domain and under the following license: you are granted a perpetual, irrevocable license to copy, modify, publish, and distribute this file as you see fit.

## Current Limitations
- It doesn't support templates.
- There is currently no introspection support for functions.
- Only limited support for introspection of C++ STL containers. These are:
    - std::vector
    - std::string
    - std::list
    - std::forward_list
    - std::deque.
