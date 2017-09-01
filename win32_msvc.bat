@echo off

rem Variables to set.
set RELEASE=false
set RUN_CODE_AFTER_BUILDING=true

set RUN_TEST=true

rem Warnings to ignore.
set COMMON_WARNINGS=-wd4189 -wd4706 -wd4996 -wd4100 -wd4127 -wd4267 -wd4505 -wd4820 -wd4365 -wd4514 -wd4062 -wd4061 -wd4668 -wd4389 -wd4018 -wd4711 -wd4987 -wd4710 -wd4625 -wd4626 -wd4350 -wd4826 -wd4640 -wd4571 -wd4986 -wd4388 -wd4129 -wd4201 -wd4577 -wd4244 -wd4623 -wd4204 -wd4101 -wd4255 -wd4191 -wd4477

rem Debug/Release
set DEBUG_COMMON_COMPILER_FLAGS=-nologo -MTd -Gm- -GR- -EHsc- -Od -Oi %COMMON_WARNINGS% -DINTERNAL=1 -DMEM_CHECK=0 -FC -Zi -GS- -Gs9999999
set RELEASE_COMMON_COMPILER_FLAGS=-nologo -MT -fp:fast -Gm- -GR- -EHa- -O2 -Oi %COMMON_WARNINGS% -DINTERNAL=0 -DMEM_CHECK=0 -FC -Zi -GS- -Gs9999999

IF NOT EXIST "build" mkdir "build"
pushd "build"
if "%RELEASE%"=="true" (
    cl -Femirror %RELEASE_COMMON_COMPILER_FLAGS% -GS- -Gs9999999 -Wall ../src/build.cpp -link -subsystem:console,5.2 kernel32.lib -stack:0x100000,0x100000
) else (
    cl -Femirror %DEBUG_COMMON_COMPILER_FLAGS% -GS- -Gs9999999 -Wall ../src/build.cpp -link -subsystem:console,5.2 kernel32.lib -stack:0x100000,0x100000
    mirror -t
)
popd

if "%RUN_TEST%"=="true" (
    pushd "game"
    "../build/mirror.exe" game.cpp
    popd

    pushd "build"
    cl -Fegame %DEBUG_COMMON_COMPILER_FLAGS% -Wall "../game/game.cpp" -FmGame.map -link -subsystem:windows,5.2 kernel32.lib
    popd
)
