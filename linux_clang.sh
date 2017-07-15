#!/bin/bash

CLANG_VERSION=3.8
RELEASE=false
RUN_TEST=true

# Mirror
WARNINGS="-Wno-unused-function -Wno-unused-variable -Wno-switch -Wno-sign-compare -Wno-unused-parameter -Wno-writable-strings -Wno-unknown-escape-sequence -Wno-missing-field-initializers -Wno-missing-braces" 

echo "Building mirror"
FILES=""src/utilities.c" "src/lexer.c" "src/write_file.c" "src/platform_linux.c" "src/main.c" "src/test.c""
if [ "$RELEASE" = "true" ]; then
    clang-"$CLANG_VERSION" -Wall -Wextra $FILES -std=c99 -fno-exceptions -fno-rtti -o mirror_exe -DINTERNAL=0 $WARNINGS -ldl
else
    clang-"$CLANG_VERSION" -Wall -Wextra $FILES -std=c99 -fno-exceptions -fno-rtti -o mirror_exe -DINTERNAL=1 $WARNINGS -g -ldl
fi
mv "./mirror_exe" "build/mirror"

# Run tests
if [ "$RELEASE" = "false" ]; then
    build/mirror -t    
fi

# Test.
#if [ "$RUN_TEST" = "true" ]; then
#    echo "Building Test"
#    pushd "test"
#    "../build/mirror" -c test_code.c
#    popd
#
#    clang-"$CLANG_VERSION" -Wall -Wextra "test/test_code.c" -std=c99 -o test_exe -Wno-unused-parameter -Wno-unused-function -Wno-unused-variable -Wno-switch -Wno-sign-compare -Wno-unused-private-field -Wno-unused-parameter -g -ldl
#    mv "./test_exe" "build/test"
#fi

# Math Test.
if [ "$RUN_TEST" = "true" ]; then
    echo "Building Math"
    pushd "test"
    "../build/mirror" -c sgl_math.c
    popd

    clang-"$CLANG_VERSION" -Wall -Wextra "test/sgl_math.c" -std=c99 -o test_exe -Wno-unused-parameter -Wno-unused-function -Wno-unused-variable -Wno-switch -Wno-sign-compare -Wno-unused-private-field -Wno-unused-parameter -g -ldl
    mv "./test_exe" "build/test"
fi
