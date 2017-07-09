@echo off

rem Variables to set.
set RELEASE=false
set RUN_CODE_AFTER_BUILDING=true

set RUN_TEST=true

rem Warnings to ignore.
set COMMON_WARNINGS=-wd4189 -wd4706 -wd4996 -wd4100 -wd4127 -wd4267 -wd4505 -wd4820 -wd4365 -wd4514 -wd4062 -wd4061 -wd4668 -wd4389 -wd4018 -wd4711 -wd4987 -wd4710 -wd4625 -wd4626 -wd4350 -wd4826 -wd4640 -wd4571 -wd4986 -wd4388 -wd4129 -wd4201 -wd4577 -wd4244 -wd4623 -wd4204 -wd4101 -wd4255

rem call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" x64

rem Debug/Release
set DEBUG_COMMON_COMPILER_FLAGS=-nologo -MTd -Gm- -GR- -EHsc- -Od -Oi %COMMON_WARNINGS% -DINTERNAL=1 -DMEM_CHECK=0 -FC -Zi -GS- -Gs9999999
set RELEASE_COMMON_COMPILER_FLAGS=-nologo -MT -fp:fast -Gm- -GR- -EHa- -O2 -Oi %COMMON_WARNINGS% -DINTERNAL=0 -DMEM_CHECK=0 -FC -Zi -GS- -Gs9999999

set FILES="../src/utilities.c" "../src/lexer.c" "../src/write_file.c" "../src/platform_win32.c" "../src/main.c"
IF NOT EXIST "build" mkdir "build"
pushd "build"
if "%RELEASE%"=="true" (
    cl -Femirror %RELEASE_COMMON_COMPILER_FLAGS% -GS- -Gs9999999 -Wall %FILES% -link -nodefaultlib -subsystem:console,5.2 kernel32.lib -stack:0x100000,0x100000
) else (
    cl -Femirror %DEBUG_COMMON_COMPILER_FLAGS% -GS- -Gs9999999 -Wall %FILES% "../src/test.c" -link -nodefaultlib -subsystem:console,5.2 kernel32.lib -stack:0x100000,0x100000
    mirror -t
)
popd

rem Run after building.

rem Test code for C++.
rem if "%RUN_TEST%"=="true" (
rem     pushd "test"
rem     "../build/mirror.exe" test_code.cpp
rem     popd
rem 
rem     pushd "build"
rem     cl -Fetest %DEBUG_COMMON_COMPILER_FLAGS% -Wall "../test/test_code.cpp" -FmTest.map -link -subsystem:console,5.2 kernel32.lib
rem     popd
rem )
rem 
rem rem Test code for C.
rem if "%RUN_TEST%"=="true" (
rem     pushd "test"
rem     "../build/mirror.exe" -c test_code.c
rem     popd
rem 
rem     pushd "build"
rem     cl -Fetest %DEBUG_COMMON_COMPILER_FLAGS% -Wall "../test/test_code.c" -FmTest.map -link -subsystem:console,5.2 kernel32.lib
rem     popd
rem )

rem Math test
if "%RUN_TEST%"=="true" (
    pushd "test"
    "../build/mirror.exe" -p sgl_math.c
    popd

    pushd "build"
    cl -Fetest %DEBUG_COMMON_COMPILER_FLAGS% -Wall "../test/sgl_math.c" -FmTest.map -link -subsystem:console,5.2 kernel32.lib
    popd
)

echo "Done"