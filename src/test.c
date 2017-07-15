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

#include "types.h"
#include "test.h"
#include "utilities.h"
#include "lexer.h"

Struct_Data parse_struct_test(Char *str, Struct_Type type) {
    Tokenizer tokenizer = {str};

    eat_token(&tokenizer);
    Parse_Struct_Result res = parse_struct(&tokenizer, type);

    return(res.sd);
}

Int struct_test_name(void) {
    Int res = 0;

    {
        Struct_Data sd = parse_struct_test("struct Foo {};", StructType_struct);
        if(!cstring_string_comp("Foo", sd.name)) {
            push_error(ErrorType_incorrect_struct_name);
            ++res;
        }
    }

    {
        Struct_Data sd = parse_struct_test("struct Foo { int a, b, c; };", StructType_struct);
        if(!cstring_string_comp("Foo", sd.name)) {
            push_error(ErrorType_incorrect_struct_name);
            ++res;
        }
    }

    {
        Struct_Data sd = parse_struct_test("struct Bar { int a; int b; int c; };", StructType_struct);
        if(!cstring_string_comp("Bar", sd.name)) {
            push_error(ErrorType_incorrect_struct_name);
            ++res;
        }
    }

    {
        Struct_Data sd = parse_struct_test("class Foo {};", StructType_struct);
        if(!cstring_string_comp("Foo", sd.name)) {
            push_error(ErrorType_incorrect_struct_name);
            ++res;
        }
    }

    {
        Struct_Data sd = parse_struct_test("class FooBar : public Foo, public Bar {};", StructType_struct);
        if(!cstring_string_comp("FooBar", sd.name)) {
            push_error(ErrorType_incorrect_struct_name);
            ++res;
        }
    }

    return(res);
}

Int struct_test_member_count(void) {
    Int res = 0;

    {
        Struct_Data sd =  parse_struct_test("struct Foo { int a; int b; int c;};", StructType_struct);

        if(sd.member_count != 3) {
            push_error(ErrorType_incorrect_number_of_members_for_struct);
            ++res;
        }
    }

    {
        Struct_Data sd =  parse_struct_test("struct Foo { int a, b, c;};", StructType_struct);

        if(sd.member_count != 3) {
            push_error(ErrorType_incorrect_number_of_members_for_struct);
            ++res;
        }
    }

    {
        Struct_Data sd =  parse_struct_test("struct Foo { int a; float *b; char **c; double d[10]; short *e[10]; };", StructType_struct);

        if(sd.member_count != 5) {
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
        Struct_Data sd =  parse_struct_test(s, StructType_struct);

        if(sd.member_count != 5) {
            push_error(ErrorType_incorrect_number_of_members_for_struct);
            ++res;
        }
    }

    return(res);
}

Int struct_test_inheritance_count(void) {
    Int res = 0;

    {
        Struct_Data sd = parse_struct_test("struct FooBar : public Foo, public Bar {};", StructType_struct);
        if(sd.inherited_count != 2) {
            push_error(ErrorType_incorrect_number_of_base_structs);
            ++res;
        }
    }

    {
        Struct_Data sd = parse_struct_test("struct FooBar : public Foo {};", StructType_struct);
        if(sd.inherited_count != 1) {
            push_error(ErrorType_incorrect_number_of_base_structs);
            ++res;
        }
    }

    {
        Struct_Data sd = parse_struct_test("struct FooBar : public F, public O, public O, public B, public A, public R {};", StructType_struct);
        if(sd.inherited_count != 6) {
            push_error(ErrorType_incorrect_number_of_base_structs);
            ++res;
        }
    }

    return(res);
}

Int struct_test_members(void) {
    Int res = 0;

    Struct_Data sd = parse_struct_test("struct Foo { int a; float b; char *c; short *d[10]; };", StructType_struct);
    Variable a = create_variable("int",    "a", 0, 0);
    Variable b = create_variable("float",  "b", 0, 0);
    Variable c = create_variable("char",   "c", 1, 0);
    Variable d = create_variable("short",  "d", 1, 10);

    if(!variable_comp(a, sd.members[0])) {
        push_error(ErrorType_incorrect_members_in_struct);
        ++res;
    }

    if(!variable_comp(b, sd.members[1])) {
        push_error(ErrorType_incorrect_members_in_struct);
        ++res;
    }

    if(!variable_comp(c, sd.members[2])) {
        push_error(ErrorType_incorrect_members_in_struct);
        ++res;
    }

    if(!variable_comp(d, sd.members[3])) {
        push_error(ErrorType_incorrect_members_in_struct);
        ++res;
    }

    return(res);
}

Int struct_test_type(void) {
    Int res = 0;

    {
        Struct_Data sd = parse_struct_test("struct Foo {};", StructType_struct);
        if(sd.struct_type != StructType_struct) {
            push_error(ErrorType_incorrect_data_structure_type);
            ++res;
        }
    }

    {
        Struct_Data sd = parse_struct_test("class Foo {};", StructType_class);
        if(sd.struct_type != StructType_class) {
            push_error(ErrorType_incorrect_data_structure_type);
            ++res;
        }
    }

    {
        Struct_Data sd = parse_struct_test("union Foo {};", StructType_union);
        if(sd.struct_type != StructType_union) {
            push_error(ErrorType_incorrect_data_structure_type);
            ++res;
        }
    }

    return(res);
}

Int struct_tests(void) {
    Int res = 0;

    res += struct_test_member_count();
    res += struct_test_name();
    res += struct_test_inheritance_count();
    res += struct_test_members();
    res += struct_test_type();

    return(res);
}

Int run_tests(void) {
    Int res = 0;

    res += struct_tests();

    return(res);
}