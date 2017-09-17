#!/bin/bash

CLANG_VERSION=3.8
RELEASE=false
BUILD_GAME=true

# Mirror
WARNINGS="-Wno-unused-function -Wno-unused-variable -Wno-switch -Wno-sign-compare -Wno-unused-parameter -Wno-writable-strings -Wno-unknown-escape-sequence -Wno-missing-field-initializers -Wno-missing-braces -Wno-char-subscripts" 

echo "Building mirror"
if [ "$RELEASE" = "true" ]; then
    clang++-"$CLANG_VERSION" -Wall -Wextra src/build.cpp -std=c++1y -fno-exceptions -fno-rtti -o mirror_exe -DINTERNAL=0 $WARNINGS -ldl
else
    clang++-"$CLANG_VERSION" -Wall -Wextra src/build.cpp -std=c++1y -fno-exceptions -fno-rtti -o mirror_exe -DINTERNAL=1 $WARNINGS -g -ldl
fi
mv "./mirror_exe" "build/mirror"

# Run tests
if [ "$RELEASE" = "false" ]; then
    build/mirror -t    
fi

if [ "$BUILD_GAME" = "true" ]; then
    echo "Building Game"
    pushd "game"
    "../build/mirror" game.cpp
    popd

    clang-"$CLANG_VERSION" -Wall -Wextra "game/game.cpp" -std=c++1y -o game_exe -Wno-unused-parameter -Wno-unused-function -Wno-unused-variable -Wno-switch -Wno-sign-compare -Wno-unused-private-field -Wno-unused-parameter -Wno-char-subscripts -g -ldl
    mv "./game_exe" "build/game"
fi
