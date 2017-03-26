/*===================================================================================================
  File:                    test.cpp
  Author:                  Jonathan Livingstone
  Email:                   seagull127@ymail.com
  Licence:                 Public Domain
                           No Warranty is offered or implied about the reliability,
                           suitability, or usability
                           The use of this code is at your own risk
                           Anyone can use this code, modify it, sell it to terrorists, etc.
  ===================================================================================================*/

#if INTERNAL

#include "platform.h"
#include "utils.h"
namespace {
#include "lexer.cpp"
}

internal StructData parse_struct_test(Char *str, StructType type = StructType_struct) {
    Tokenizer t = {str};

    eat_token(&t);
    ParseStructResult res = parse_struct(&t, type);

    return(res.sd);
}

internal Int struct_tests() {
    Int res = 0;

    //
    // Member Count.
    //
    {
        StructData sd =  parse_struct_test("struct Foo { int a, b, c;};");

        if(sd.member_count != 3) {
            push_error(ErrorType_incorrect_number_of_members_for_struct);
            ++res;
        }
    }

    {
        StructData sd =  parse_struct_test("struct Foo { int a; float f; double d; short s;};");

        if(sd.member_count != 4) {
            push_error(ErrorType_incorrect_number_of_members_for_struct);
            ++res;
        }
    }

    {
        Char *s = "struct Foo {;"
                  "    char const *str1;"
                  "    const char *str2;"
                  "    int unsigned len1;"
                  "    unsigned int len2;"
                  "    int **ptr;"
                  "};";
        StructData sd =  parse_struct_test(s);

        if(sd.member_count != 5) {
            push_error(ErrorType_incorrect_number_of_members_for_struct);
            ++res;
        }
    }

    //
    // Name.
    //
    {
        StructData sd = parse_struct_test("struct FooBar : public Foo, public Bar {};");
        if(sd.inherited_count != 2) {
            push_error(ErrorType_incorrect_number_of_base_structs);
            ++res;
        }
    }

    {
        StructData sd = parse_struct_test("struct FooBar : public Foo {};");
        if(sd.inherited_count != 1) {
            push_error(ErrorType_incorrect_number_of_base_structs);
            ++res;
        }
    }

    {
        StructData sd = parse_struct_test("struct FooBar : public F, public O, public O, public B, public A, public R {};");
        if(sd.inherited_count != 6) {
            push_error(ErrorType_incorrect_number_of_base_structs);
            ++res;
        }
    }

    //
    // Inherited Count.
    //
    {
        StructData sd = parse_struct_test("struct FooBar : public Foo, public Bar {};");
        if(sd.inherited_count != 2) {
            push_error(ErrorType_incorrect_number_of_base_structs);
            ++res;
        }
    }

    {
        StructData sd = parse_struct_test("struct FooBar : public Foo {};");
        if(sd.inherited_count != 1) {
            push_error(ErrorType_incorrect_number_of_base_structs);
            ++res;
        }
    }

    {
        StructData sd = parse_struct_test("struct FooBar : public F, public O, public O, public B, public A, public R {};");
        if(sd.inherited_count != 6) {
            push_error(ErrorType_incorrect_number_of_base_structs);
            ++res;
        }
    }

    return(res);
}

#endif // INTERNAL

Int run_tests() {
    Int res = 0;
#if INTERNAL
    res += struct_tests();
#endif

    return(res);
}
