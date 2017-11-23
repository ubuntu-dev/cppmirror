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

    clang-"$CLANG_VERSION" -Wall -Wextra "game/game.c" -std=c99 -o game_exe $WARNINGS -g -ldl -lm
    mv "./game_exe" "build/game"
fi
