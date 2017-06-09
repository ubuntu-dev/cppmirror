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

StructData parse_struct_test(Char *str, StructType type = StructType_struct) {
    Tokenizer tokenizer = {str};

    eat_token(&tokenizer);
    ParseStructResult res = parse_struct(&tokenizer, type);

    return(res.sd);
}

Int struct_test_name() {
    Int res = 0;

    {
        StructData sd = parse_struct_test("struct Foo {};");
        if(!string_comp("Foo", sd.name)) {
            push_error(ErrorType_incorrect_struct_name);
            ++res;
        }
    }

    {
        StructData sd = parse_struct_test("struct Foo { int a, b, c; };");
        if(!string_comp("Foo", sd.name)) {
            push_error(ErrorType_incorrect_struct_name);
            ++res;
        }
    }

    {
        StructData sd = parse_struct_test("struct Bar { int a; int b; int c; };");
        if(!string_comp("Bar", sd.name)) {
            push_error(ErrorType_incorrect_struct_name);
            ++res;
        }
    }

    {
        StructData sd = parse_struct_test("class Foo {};");
        if(!string_comp("Foo", sd.name)) {
            push_error(ErrorType_incorrect_struct_name);
            ++res;
        }
    }

    {
        StructData sd = parse_struct_test("class FooBar : public Foo, public Bar {};");
        if(!string_comp("FooBar", sd.name)) {
            push_error(ErrorType_incorrect_struct_name);
            ++res;
        }
    }

    return(res);
}

Int struct_test_member_count() {
    Int res = 0;

    {
        StructData sd =  parse_struct_test("struct Foo { int a; int b; int c;};");

        if(sd.member_count != 3) {
            push_error(ErrorType_incorrect_number_of_members_for_struct);
            ++res;
        }
    }

    {
        StructData sd =  parse_struct_test("struct Foo { int a, b, c;};");

        if(sd.member_count != 3) {
            push_error(ErrorType_incorrect_number_of_members_for_struct);
            ++res;
        }
    }

    {
        StructData sd =  parse_struct_test("struct Foo { int a; float *b; char **c; double d[10]; short *e[10]; };");

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
        StructData sd =  parse_struct_test(s);

        if(sd.member_count != 5) {
            push_error(ErrorType_incorrect_number_of_members_for_struct);
            ++res;
        }
    }

    return(res);
}

Int struct_test_inheritance_count() {
    Int res = 0;

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

Int struct_test_members() {
    Int res = 0;

    {
        StructData sd = parse_struct_test("struct Foo { int a; float b; char *c; short *d[10]; };");
        Variable a = create_variable("int",    "a");
        Variable b = create_variable("float",  "b");
        Variable c = create_variable("char",   "c", 1);
        Variable d = create_variable("short",  "d", 1, 10);

        if((!compare_variable(a, sd.members[0])) ||
                (!compare_variable(b, sd.members[1])) ||
                (!compare_variable(c, sd.members[2])) ||
                (!compare_variable(d, sd.members[3]))) {
            push_error(ErrorType_incorrect_members_in_struct);
            ++res;
        }
    }

    return(res);
}

Int struct_test_type() {
    Int res = 0;

    {
        StructData sd = parse_struct_test("struct Foo {};");
        if(sd.struct_type != StructType_struct) {
            push_error(ErrorType_incorrect_data_structure_type);
            ++res;
        }
    }

    {
        StructData sd = parse_struct_test("class Foo {};");
        if(sd.struct_type != StructType_class) {
            push_error(ErrorType_incorrect_data_structure_type);
            ++res;
        }
    }

    {
        StructData sd = parse_struct_test("union Foo {};");
        if(sd.struct_type != StructType_union) {
            push_error(ErrorType_incorrect_data_structure_type);
            ++res;
        }
    }

    return(res);
}

Int struct_tests() {
    Int res = 0;

    res += struct_test_member_count();
    res += struct_test_name();
    res += struct_test_inheritance_count();
    res += struct_test_members();
    res += struct_test_type();

    return(res);
}
