#!/bin/bash

CLANG_VERSION=3.8
RELEASE=false
BUILD_GAME=true

# Mirror
WARNINGS="-Wno-unused-function -Wno-unused-variable -Wno-switch -Wno-sign-compare -Wno-unused-parameter -Wno-writable-strings -Wno-unknown-escape-sequence -Wno-missing-field-initializers -Wno-missing-braces -Wno-char-subscripts -Wno-typedef-redefinition"  

echo "Building mirror"
if [ "$RELEASE" = "true" ]; then
    clang-"$CLANG_VERSION" -Wall -Wextra src/build.c -std=c99 -fno-exceptions -fno-rtti -o mirror_exe -DINTERNAL=0 $WARNINGS -ldl
else
    clang-"$CLANG_VERSION" -Wall -Wextra src/build.c -std=c99 -fno-exceptions -fno-rtti -o mirror_exe -DINTERNAL=1 $WARNINGS -g -ldl 
fi
mv "./mirror_exe" "build/mirror"

# Run tests
if [ "$RELEASE" = "false" ]; then
    build/mirror -t    
fi

if [ "$BUILD_GAME" = "true" ]; then
    echo "Building Game"
    pushd "game"
    "../build/mirror" game.c
    popd

    clang-"$CLANG_VERSION" -Wall -Wextra -shared -fPIC "game/game.c" -std=gnu99 -o game_app $WARNINGS -g  `sdl2-config --cflags --libs` -ldl -lm -lrt
    clang-"$CLANG_VERSION" -Wall -Wextra -DSGLP_LOAD_GAME_FROM_DLL "game/linux_platform.c" -std=gnu99 -o platform_app $WARNINGS -g  `sdl2-config --cflags --libs` -ldl -lm -lrt
    mv "./game_app" "build/game"
    mv "./platform_app" "build/platform"
fi
