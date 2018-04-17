@echo off

rem Setup Visual Studio 2017.
call "C:\Program Files (x86)\Microsoft Visual Studio\2017\community\VC\Auxiliary\Build\vcvarsall.bat" x64 > NUL

cd %~dp0

rem Variables to set.
set RELEASE=false
set BUILD_MIRROR=true
set BUILD_GAME=true
set RUN_TESTS=true

rem Warnings to ignore.
set COMMON_WARNINGS=-wd4189 -wd4706 -wd4996 -wd4100 -wd4127 -wd4267 -wd4505 -wd4820 -wd4365 -wd4514 -wd4062 -wd4061 -wd4668 -wd4389 -wd4018 -wd4711 -wd4987 -wd4710 -wd4625 -wd4626 -wd4350 -wd4826 -wd4640 -wd4571 -wd4986 -wd4388 -wd4129 -wd4201 -wd4577 -wd4244 -wd4623 -wd4204 -wd4101 -wd4255 -wd4191 -wd4477 -wd4242

rem INTERNAL macro
if "%RELEASE%"=="true" (
    set INTERNAL=-DINTERNAL=0
) else (
    set INTERNAL=-DINTERNAL=1
)

rem Compiler flags
set COMMON_COMPILER_FLAGS=-nologo -Gm- -GR- %COMMON_WARNINGS% -FC -Zi -Oi -GS- -Gs9999999 -Wall %INTERNAL%
if "%RELEASE%"=="true" (
    set COMPILER_FLAGS=%COMMON_COMPILER_FLAGS% -MT -fp:fast -EHa- -O2
) else (
    set COMPILER_FLAGS=%COMMON_COMPILER_FLAGS% -MTd -EHsc- -Od
)

rem Linker flags.
set COMMON_LINKER_FLAGS=-link -nodefaultlib kernel32.lib -stack:0x100000,0x100000

rem Make build directory.
IF NOT EXIST "build" mkdir "build"

rem Build Mirror
if "%BUILD_MIRROR%"=="true" (
    pushd "build"
    cl -Femirror %COMPILER_FLAGS% "../src/build.c" -FmMirror.map %COMMON_LINKER_FLAGS% -subsystem:console,5.2
    popd
)

rem Run Tests on Mirror
if "%RUN_TESTS%"=="true" (
    "build/mirror.exe" -t
)

rem Build game
if "%BUILD_GAME%"=="true" (
    rem Run mirror on game code.
    pushd "game"
    "../build/mirror.exe" game.c %INTERNAL%
    popd

    rem Compile game code.
    pushd "build"

    rem This is the version where the platform code and game code are a seperate EXE and DLL.
    cl -FeWin32 %COMPILER_FLAGS% -DSGLP_LOAD_GAME_FROM_DLL=game.dll "../game/win32_platform.c" -FmWin32.map %COMMON_LINKER_FLAGS% -subsystem:windows,5.2
    cl -LD -FeGame %COMPILER_FLAGS% "../game/game.c" -FmGame.map %COMMON_LINKER_FLAGS%  -EXPORT:sglp_platform_update_and_render_callback -EXPORT:sglp_platform_setup_settings_callback -subsystem:windows,5.2
    
    rem This is the version with the platform and the game code in one EXE.
    rem cl -FeGame %COMPILER_FLAGS% "../game/game.c" -FmGame.map %COMMON_LINKER_FLAGS% -subsystem:windows,5.2
    popd
)
