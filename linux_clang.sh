#!/bin/bash

CLANG_VERSION=3.8
RELEASE=false
RUN_TEST=true

# Mirror
WARNINGS="-Wno-unused-function -Wno-unused-variable -Wno-c++11-compat-deprecated-writable-strings -Wno-switch -Wno-sign-compare -Wno-unused-parameter -Wno-writable-strings -Wno-unknown-escape-sequence"

FILES=""src/main.cpp" "src/utils.cpp" "src/lexer.cpp" "src/platform_linux.cpp" "src/write_file.cpp" "src/test.cpp""

echo "Building mirror"
if [ "$RELEASE" = "true" ]; then
    clang++-"$CLANG_VERSION" -Wall -Wextra $FILES -std=c++1z -fno-exceptions -fno-rtti -o mirror_exe -DINTERNAL=0 $WARNINGS -ldl
else
    clang++-"$CLANG_VERSION" -Wall -Wextra $FILES -std=c++1z -fno-exceptions -fno-rtti -o mirror_exe -DINTERNAL=1 $WARNINGS -g -ldl
fi
mv "./mirror_exe" "build/mirror"

# Run tests
if [ "$RELEASE" = "false" ]; then
    build/mirror -t    
fi

# Test.
if [ "$RUN_TEST" = "true" ]; then
    echo "Building Test"
    pushd "test"
    "../build/mirror" test_code.cpp
    popd

    clang++-"$CLANG_VERSION" -Wall -Wextra "test/test_code.cpp" -std=c++1z -o test_exe -Wno-unused-parameter -Wno-unused-function -Wno-unused-variable -Wno-c++11-compat-deprecated-writable-strings -Wno-switch -Wno-sign-compare -Wno-unused-private-field -Wno-unused-parameter -g -ldl
    mv "./test_exe" "build/test"
fi
