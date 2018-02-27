# Mirror
## Overview

This is a simple project which aims to provide some meta programming functionality not currently available in C++ through code generation. All generated code strictly follows the C++11 specification, and should compile correctly under MSVC, Clang, or GCC, even with `-Wall` enabled.

## Directory Structure
`build` - A generated folder, which will hold the executable for Mirror and the Game.

`game` - A folder holding all the source code, and assets, for the demo game being create along side Mirror. The only file that gets built in here is `game.cpp`, which just `#include`s all the other `.cpp` and `.h` files directly.

`prebuild` - Holds prebuild executables for mirror.

`src` - The source code for mirror. The only file in here that is actually compiled is `build.cpp`, which `#include`s all of the other `.cpp` and `.h` files directly. On Windows, which also includes the `platform_win32.cpp` file, and on Linux it will include
`platform_linux.cpp`.

`linux_clang.sh` - Shell script file, which will build Mirror on Linux using Clang.

`win32_msvc.bat` - Batch file, which will build Mirror on Windows using MSVC.

## Build instructionse

### Windows - MSVC
Call `win32_msvc.bat` from the command line after previously calling 'vcvarsal.bat' in your favourite version of Visual Studio. This will generate the executable inside the `build` directory. This will also build the game.

### Linux - Clang
Call `linux_clang.sh` from the command line, which should generate the app file inside the `build` directory. This uses Clang version 3.8, so if you don't have that version installed on your computer, then you'll need to change the line that says `CLANG_VERSION=3.8` to match the version you've got. This also builds the game.

### Mac
Not currently supported.

## Contact

Any bugs, suggestions, complaints, or just general feedback, should be emailed to: jonathanglivingstone@gmail.com.

## LICENSE

This software is dual-licensed to the public domain and under the following license: you are granted a perpetual, irrevocable license to copy, modify, publish, and distribute this file as you see fit.

## Current Limitations
- It doesn't support templates. For simple ones, the code should just ignore them.
- There is currently no introspection support for functions.
- Doesnt support the C++ Standard Template Library, because the STL is _really_ shit.
