/*===================================================================================================
  File:                    lexer.cpp
  Author:                  Jonathan Livingstone
  Email:                   seagull127@ymail.com
  Licence:                 Public Domain
                           No Warranty is offered or implied about the reliability,
                           suitability, or usability
                           The use of this code is at your own risk
                           Anyone can use this code, modify it, sell it to terrorists, etc.
  ===================================================================================================*/

enum Modifier {
    Modifier_unknown  = 0x00,

    Modifier_unsigned = 0x01,
    Modifier_const    = 0x02,
    Modifier_volatile = 0x04,
    Modifier_mutable  = 0x08,
    Modifier_signed   = 0x10,
};

enum Struct_Type {
    StructType_unknown,
    StructType_struct,
    StructType_class,
    StructType_union,

    StructType_count
};

struct Struct_Data {
    String template_header;
    String name;
    Int member_count;
    Variable *members;

    Int inherited_count;
    String *inherited;

    Struct_Type struct_type;
};

struct Structs {
    Struct_Data *e;
    Int cnt;
};

struct Enum_Value {
    String name;
    Int value;
};

struct Enum_Data {
    String name;
    String type;
    Bool is_struct;

    Enum_Value *values;
    Int no_of_values;
};

struct Enums {
    Enum_Data *e;
    Int cnt;
};

struct Function_Data {
    String linkage;
    String return_type;
    Int return_type_ptr;
    String name;

    Variable *params;
    Int param_cnt;
};

struct Typedef_Data {
    String original;
    String fresh;
};

struct Functions {
    Function_Data *e;
    Int cnt;
};

struct Typedefs {
    Typedef_Data *e;
    Int cnt;
};

struct Parse_Result {
    Structs structs;
    Enums enums;
    Functions funcs;
    Typedefs typedefs;

    Int struct_max;
    Int enum_max;
    Int func_max;
    Int typedef_max;
};

enum Token_Type {
    Token_Type_unknown,

    Token_Type_open_paren,
    Token_Type_close_paren,
    Token_Type_colon,
    Token_Type_semi_colon,
    Token_Type_asterisk,
    Token_Type_open_bracket,
    Token_Type_close_bracket,
    Token_Type_open_brace,
    Token_Type_close_brace,
    Token_Type_open_angle_bracket,
    Token_Type_close_angle_bracket,
    Token_Type_hash,
    Token_Type_assign,
    Token_Type_comma,
    Token_Type_tilde,
    Token_Type_period,
    Token_Type_ampersand,
    Token_Type_inclusive_or,
    Token_Type_not,

    Token_Type_plus,
    Token_Type_minus,
    Token_Type_divide,

    Token_Type_equal,
    Token_Type_not_equal,
    Token_Type_greater_than_or_equal,
    Token_Type_less_than_or_equal,

    Token_Type_number,
    Token_Type_identifier,
    Token_Type_string,
    Token_Type_var_args,

    Token_Type_error,

    Token_Type_end_of_stream,

    Token_Type_count
};

struct Token {
    Char *e;
    Uintptr len;

    Token_Type type;
};

struct Tokenizer {
    Char *at;
};

Bool is_end_of_line(Char c) {
    Bool res = ((c == '\n') || (c == '\r'));

    return(res);
}

Bool is_whitespace(Char c) {
    Bool res = ((c == ' ') || (c == '\t') || (c == '\v') || (c == '\f') || (is_end_of_line(c)));

    return(res);
}

Bool is_whitespace_no_end_line(Char c) {
    Bool res = ((c == ' ') || (c == '\t') || (c == '\v') || (c == '\f'));

    return(res);
}


Void skip_to_end_of_line(Tokenizer *tokenizer) {
    while(is_end_of_line(*tokenizer->at)) {
        ++tokenizer->at;
    }
}

String token_to_string(Token token) {
    String res = { token.e, token.len };

    return(res);
}

Void token_to_cstring(Token token, Char *buf, Int size) {
    assert(size > token.len);
    for(Int i = 0; (i < token.len); ++i, ++buf) {
        *buf = token.e[i];
    }

    *buf = 0;
}

Bool token_compare(Token a, Token b) {
    Bool res = false;

    if(a.len == b.len) {
        res = true;

        for(Int i = 0; (i < a.len); ++i) {
            if(a.e[i] != b.e[i]) {
                res = false;
                break; // for
            }
        }
    }

    return(res);
}

Bool token_compare_string(Token a, String b) {
    Bool res = false;

    if(a.len == b.len) {
        res = true;

        for(Int i = 0; (i < a.len); ++i) {
            if(a.e[i] != b.e[i]) {
                res = false;
                break; // for
            }
        }
    }

    return(res);
}

Bool token_compare_cstring_len(Token a, Char *b, Uintptr string_len) {
    Bool res = true;

    if(!string_len) {
        string_len = a.len;
    }

    for(Int i = 0; (i < string_len); ++i) {
        if(a.e[i] != b[i]) {
            res = false;
            break;
        }
    }

    return(res);
}

Bool token_compare_cstring(Token a, Char *b) {
    Uintptr len = a.len;
    Bool res = token_compare_cstring_len(a, b, len);

    return(res);
}

Modifier is_modifier(Token token) {
    Modifier res = {};

    struct ModifierWithString {
        String str;
        Modifier mod;
    };

#define CREATE_MODIFIER(name) { create_string(#name), Modifier_##name }

    ModifierWithString modifiers[] = {
        CREATE_MODIFIER(unsigned),
        CREATE_MODIFIER(const),
        CREATE_MODIFIER(volatile),
        CREATE_MODIFIER(mutable),
        CREATE_MODIFIER(signed)
    };

    for(Int i = 0; (i < array_count(modifiers)); ++i) {
        ModifierWithString *mod = modifiers + i;

        if(token_compare_string(token, mod->str)) {
            res = mod->mod;
        }
    }

    return(res);
}

ResultInt token_to_int(Token t) {
    String str = token_to_string(t);
    ResultInt res = string_to_int(str);

    return(res);
}

Bool token_equals(Token token, Char *str) {
    Bool res = false;

    Int len = string_length(str);
    if(len == token.len) {
        res = string_comp_len(token.e, str, len);
    }

    return(res);
}

Token get_token(Tokenizer *tokenizer); // Because C/C++...
Token peak_token(Tokenizer *tokenizer) {
    Tokenizer cpy = *tokenizer;
    Token res = get_token(&cpy);

    return(res);
}

#define eat_token(tokenizer) eat_tokens(tokenizer, 1)
Void eat_tokens(Tokenizer *tokenizer, Int num_tokens_to_eat) {
    for(Int i = 0; (i < num_tokens_to_eat); ++i) {
        get_token(tokenizer);
    }
}

Void loop_until_token(Tokenizer *tokenizer, Token_Type type) {
    Token token = get_token(tokenizer);
    while(token.type != type) {
        token = get_token(tokenizer);
    }
}

Variable parse_member(Tokenizer *tokenizer, Int var_to_parse) {
    Variable res = {0};

    Token type = get_token(tokenizer);
    if(token_equals(type, "std")) {
        // TODO(Jonny): Hacky as fuck...
        Char *std_string = "std::string";
        Int std_string_len = string_length(std_string);

        type.len = 1;
        Char *at = type.e;
        while(*at != '>') {
            ++type.len; ++at;
            if(type.len == std_string_len) {
                if(token_equals(type, "std::string")) {
                    break;
                }
            }
        }
    }

    // Modifiers
    {
        Modifier tmp = is_modifier(type);
        while(tmp) {
            res.modifier |= tmp;
            type = get_token(tokenizer);
            tmp = is_modifier(type);
        }
    }

    res.type = token_to_string(type);

    if(var_to_parse) {
        Char *at = tokenizer->at;
        int cur = 0;
        while(cur < var_to_parse) {
            Token temp = get_token(tokenizer);
            if(temp.type == Token_Type_comma) {
                ++cur;
            }
        }
    }

    Bool parsing = true;
    while(parsing) {
        Token token = get_token(tokenizer);

        {
            Modifier tmp = is_modifier(token);
            while(tmp) {
                res.modifier |= tmp;
                token = get_token(tokenizer);
                tmp = is_modifier(token);
            }
        }

        switch(token.type) {
            case Token_Type_semi_colon: case Token_Type_comma: case Token_Type_end_of_stream: {
                parsing = false;
            } break;

            case Token_Type_asterisk: {
                ++res.ptr;
            } break;

            case Token_Type_identifier: {
                res.name = token_to_string(token);
            } break;

            case Token_Type_open_bracket: {
                Token size_token = get_token(tokenizer);
                if(size_token.type == Token_Type_number) {
                    char buf[32] = {0};

                    token_to_cstring(size_token, buf, array_count(buf));
                    ResultInt arr_count = cstring_to_int(buf);
                    if(arr_count.success) {
                        res.array_count = arr_count.e;
                    } else {
                        push_error(ErrorType_failed_to_find_size_of_array);
                    }
                } else {
                    assert(0);
                }
            } break;
        }
    }

    return(res);
}

Void eat_whitespace(Tokenizer *tokenizer) {
    for(;;) {
        if(!tokenizer->at[0]) { // Nul terminator.
            break;
        } else if(is_whitespace(tokenizer->at[0])) { // Whitespace
            ++tokenizer->at;
        } else if((tokenizer->at[0] == '/') && (tokenizer->at[1] == '/')) { // C++ comments.
            tokenizer->at += 2;
            while((tokenizer->at[0]) && (!is_end_of_line(tokenizer->at[0]))) {
                ++tokenizer->at;
            }

        } else if((tokenizer->at[0] == '/') && (tokenizer->at[1] == '*')) { // C comments.
            tokenizer->at += 2;
            while((tokenizer->at[0]) && !((tokenizer->at[0] == '*') && (tokenizer->at[1] == '/'))) {
                ++tokenizer->at;
            }
            if(tokenizer->at[0] == '*') {
                tokenizer->at += 2;
            }

            // For #if 0 blocks, just ignore everything in them like a comment.
        } else if(tokenizer->at[0] == '#') { // #if 0 blocks.
            String hash_if_zero = create_string("#if 0");
            String hash_if_one = create_string("#if 1");

            if(string_comp(hash_if_zero, tokenizer->at)) {
                tokenizer->at += hash_if_zero.len;

                String hash_end_if = create_string("#endif");

                Int level = 0;
                while(++tokenizer->at) {
                    if(tokenizer->at[0] == '#') {
                        if((tokenizer->at[1] == 'i') && (tokenizer->at[2] == 'f')) {
                            ++level;

                        } else if(string_comp(hash_end_if, tokenizer->at)) {
                            if(level) {
                                --level;
                            } else {
                                tokenizer->at += hash_end_if.len;
                                eat_whitespace(tokenizer);

                                break; // for
                            }
                        }
                    }
                }

                // For #if 1 #else blocks, write everything between #else and #endif into whitespace, then
                // jump back to the beginning of the beginning of the #if 1 block to parse stuff in it.
            } else if(string_comp(hash_if_one, tokenizer->at)) { // #if 1 #else blocks.
                tokenizer->at += hash_if_one.len;
                Tokenizer cpy = *tokenizer;

                String hash_else = create_string("#else");

                String hash_end_if = create_string("#endif");

                Int level = 0;
                while(++tokenizer->at) {
                    if(tokenizer->at[0] == '#') {
                        if((tokenizer->at[1] == 'i') && (tokenizer->at[2] == 'f')) {
                            ++level;
                        } else if(string_comp(hash_end_if, tokenizer->at)) {
                            if(level != 0) {
                                --level;
                            } else {
                                tokenizer->at += hash_end_if.len;
                                break; // for
                            }
                        } else if(string_comp(hash_else, tokenizer->at)) {
                            if(level == 0) {
                                tokenizer->at += hash_else.len;
                                Int Level2 = 0;

                                while(++tokenizer->at) {
                                    if(tokenizer->at[0] == '#') {
                                        if((tokenizer->at[1] == 'i') && (tokenizer->at[2] == 'f')) {
                                            ++Level2;

                                        } else if(string_comp(hash_end_if, tokenizer->at)) {
                                            if(Level2 != 0) {
                                                --Level2;
                                            } else {
                                                tokenizer->at += hash_end_if.len;
                                                eat_whitespace(tokenizer);

                                                break; // for
                                            }
                                        }
                                    }

                                    tokenizer->at[0] = ' ';
                                }

                                break; // for
                            }
                        }
                    }

                }

                *tokenizer = cpy;
            }

            break; // for
        } else {
            break; // for
        }
    }
}

Bool is_alphabetical(Char c) {
    Bool res = (((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')));

    return(res);
}

Bool is_num(Char c) {
    Bool res = ((c >= '0') && (c <= '9'));

    return(res);
}

Void parse_number(Tokenizer *tokenizer) {
    // TODO(Jonny): Implement.
}

Bool require_token(Tokenizer *tokenizer, Token_Type desired_type) {
    Token token = get_token(tokenizer);
    Bool res = (token.type == desired_type);

    return(res);
}

Bool peak_require_token(Tokenizer *tokenizer, Char *str) {
    Bool res = false;
    Tokenizer cpy = *tokenizer;
    Token token = get_token(&cpy);

    if(string_comp_len(token.e, str, token.len)) {
        res = true;
    }

    return(res);
}

Bool peak_require_token_type(Tokenizer *tokenizer, Token_Type type) {
    Bool res = false;
    Tokenizer cpy = *tokenizer;
    Token token = get_token(&cpy);

    if(token.type == type) {
        res = true;
    }

    return(res);
}


Bool is_stupid_class_keyword(Token t) {
    Bool res = false;
    Char *keywords[] = { "private", "public", "protected" };
    for(Int i = 0, cnt = array_count(keywords); (i < cnt); ++i) {
        if(string_comp_len(keywords[i], t.e, t.len)) {
            res = true;
            goto func_exit;
        }
    }

func_exit:;

    return(res);
}

Void skip_to_matching_bracket(Tokenizer *tokenizer) {
    Int brace_count = 1;
    Token token = {0};
    Bool should_loop = true;
    while(should_loop) {
        token = get_token(tokenizer);
        switch(token.type) {
            case Token_Type_open_brace: ++brace_count; break;

            case Token_Type_close_brace: {
                --brace_count;
                if(!brace_count) {
                    should_loop = false;
                }
            } break;
        }
    }
}

String parse_template(Tokenizer *tokenizer) {
    String res = {0};

    res.e = tokenizer->at;
    eat_token(tokenizer);
    Int angle_bracket_count = 1;
    Token token;
    Bool should_loop = true;
    while(should_loop) {
        token = get_token(tokenizer);
        switch(token.type) {
            case Token_Type_open_angle_bracket: {
                ++angle_bracket_count;
            } break;

            case Token_Type_close_angle_bracket: {
                --angle_bracket_count;
                if(!angle_bracket_count) {
                    should_loop = false;
                    res.len = (tokenizer->at - res.e);
                }
            } break;
        }
    }

    return(res);
}

struct Parse_Variable_Result {
    Variable var;
    Bool success;
};
Parse_Variable_Result parse_variable(Tokenizer *tokenizer, Token_Type end_token_type_1,
                                     Token_Type end_token_type_2) {
#if INTERNAL
    Tokenizer debug_copy_tokenizer = *tokenizer;
#endif

    Parse_Variable_Result res = {0};

    Uint32 mod = {0};

    // Return type.
    Token type = get_token(tokenizer);
    if(type.type == Token_Type_identifier) {
        res.var.type = token_to_string(type);

        // Is pointer?
        Token token = get_token(tokenizer);
        while(token.type == Token_Type_asterisk) {
            ++res.var.ptr;
            token = get_token(tokenizer);
        }

        // Name.
        if((token.len) && (token.type == Token_Type_identifier)) {
            res.var.name = token_to_string(token);

            // Is array?
            token = peak_token(tokenizer);
            if((token.type != end_token_type_1) && (token.type != end_token_type_2)) {
                eat_token(tokenizer);
                if(token.type != Token_Type_open_bracket) {
                    push_error(ErrorType_failed_parsing_variable);
                } else {
                    token = get_token(tokenizer);
                    ResultInt num = token_to_int(token);
                    if(!num.success) {
                        push_error(ErrorType_failed_parsing_variable);
                    } else {
                        res.var.array_count = num.e;
                        eat_token(tokenizer); // Eat the second ']'.
                    }
                }
            } else {
                res.var.array_count = 1;
            }

            // Skip over any assignment at the end.
            // TODO(Jonny): This won't work if a variable is assigned to a function.
            if(token.type == Token_Type_assign) {
                eat_token(tokenizer);
            }

            res.success = true;
        }
    }

    return(res);
}

Bool is_forward_declared(Tokenizer *tokenizer) {
    Bool res = false;

    Tokenizer cpy = *tokenizer;
    Token token = {0};
    Bool loop = true;
    while(loop) {
        token = get_token(&cpy);
        switch(token.type) {
            case Token_Type_open_brace: {
                loop = false;
            } break;

            case Token_Type_semi_colon: {
                loop = false;
                res = true;
            } break;

            case Token_Type_end_of_stream: {
                loop = false;
            } break;
        }
    }

    return(res);
}

struct Parse_Struct_Result {
    Struct_Data sd;
    Bool success;
};
Parse_Struct_Result parse_struct(Tokenizer *tokenizer, Struct_Type struct_type) {
    Parse_Struct_Result res = {0};

    if(!is_forward_declared(tokenizer)) {
        Tokenizer start = *tokenizer;

        Access current_access = ((struct_type == StructType_struct) || (struct_type == StructType_union)) ?
                                Access_public : Access_private;
        res.sd.struct_type = struct_type;

        Bool have_name = false;
        Token name = peak_token(tokenizer);
        if(name.type == Token_Type_identifier) {
            have_name = true;
            res.sd.name = token_to_string(name);
            eat_token(tokenizer);
        }

        Token peaked_token = peak_token(tokenizer);
        if(peaked_token.type == Token_Type_colon) {
            res.sd.inherited = new String[8];

            eat_token(tokenizer);

            Token next = get_token(tokenizer);
            while(next.type != Token_Type_open_brace) {
                if(!(is_stupid_class_keyword(next)) && (next.type != Token_Type_comma)) {
                    // TODO(Jonny): Fix properly.
                    /*if(res.sd.inherited_count + 1 >= cast(Int)(get_alloc_size(res.sd.inherited) / sizeof(String))) {
                        Void *p = system_realloc(res.sd.inherited);
                        if(p) { res.sd.inherited = cast(String *)p; }
                    }*/

                    res.sd.inherited[res.sd.inherited_count++] = token_to_string(next);
                }

                next = peak_token(tokenizer);
                if(next.type != Token_Type_open_brace) {
                    eat_token(tokenizer);
                }
            }
        }

        if(require_token(tokenizer, Token_Type_open_brace)) {
            res.success = true;

            res.sd.member_count = 0;
            Int member_cnt = 0;

            typedef struct {
                Char *pos;
                Access access;
                Bool is_inside_anonymous_struct;
            } MemberInfo;

            Uintptr member_info_mem_cnt = 256;
            MemberInfo *member_info = new MemberInfo[member_info_mem_cnt];
            if(member_info) {
                Bool inside_anonymous_struct = false;
                for(;;) {
                    Token token = get_token(tokenizer);
                    if(is_stupid_class_keyword(token)) {
                        String access_as_string = token_to_string(token);
                        if(string_comp(access_as_string, create_string("public"))) {
                            current_access = Access_public;
                        } else if(string_comp(access_as_string, create_string("private"))) {
                            current_access = Access_private;
                        } else if(string_comp(access_as_string, create_string("protected"))) {
                            current_access = Access_protected;
                        }
                    } else {
                        // This could be the end of an anonymous struct, so ignore it.
                        if(token_equals(token, "struct")) {
                            eat_token(tokenizer); // Eat the open brace.
                            token = get_token(tokenizer);
                            inside_anonymous_struct = true;
                        }

                        if((token.type != Token_Type_colon) && (token.type != Token_Type_tilde)) {
                            if(token.type == Token_Type_close_brace) {
                                if(inside_anonymous_struct) {
                                    inside_anonymous_struct = false;
                                    eat_token(tokenizer); // Eat semi colon.
                                    continue;
                                } else if(!have_name) {
                                    name = get_token(tokenizer);
                                    if(name.type == Token_Type_identifier) {
                                        res.sd.name = token_to_string(name);
                                    } else {
                                        push_error(ErrorType_could_not_detect_struct_name);
                                    }
                                }

                                break; // for
                            } else if(token.type == Token_Type_hash) {
                                // TODO(Jonny): Support macros with '/' to extend their lines?
                                while(!is_end_of_line(tokenizer->at[0])) {
                                    ++tokenizer->at;
                                }
                            } else {
                                Bool is_func = false;

                                Tokenizer tokenizer_copy = *tokenizer;
                                Token temp = get_token(&tokenizer_copy);

                                while(temp.type != Token_Type_semi_colon) {

                                    // Handle C++11 assignment within struct.
                                    if(temp.type == Token_Type_assign) {
                                        loop_until_token(&tokenizer_copy, Token_Type_semi_colon);

                                        break;
                                    }

                                    // Check if it is a function.
                                    if(temp.type == Token_Type_open_paren) {
                                        is_func = true;
                                        break; // while
                                    }

                                    temp = get_token(&tokenizer_copy);
                                }

                                if(!is_func) {
                                    if(member_cnt >= member_info_mem_cnt) {
                                        member_info_mem_cnt *= 2;
                                        Void *p = system_realloc(member_info, sizeof(MemberInfo) * member_info_mem_cnt);
                                        if(p) {
                                            member_info = cast(MemberInfo *)p;
                                        }
                                    }

                                    MemberInfo *mi = member_info + member_cnt++;

                                    mi->pos = token.e;
                                    mi->is_inside_anonymous_struct = inside_anonymous_struct;
                                    mi->access = current_access;
                                } else {
                                    // Skip to the end of the function.
                                    Bool eating_func = true;
                                    Bool inside_body = false;
                                    while(eating_func) {
                                        Token t = get_token(&tokenizer_copy);
                                        if((t.type == Token_Type_semi_colon) && (!inside_body)) {
                                            break;
                                        } else if(t.type == Token_Type_open_brace) {
                                            inside_body = true;
                                        } else if((t.type == Token_Type_close_brace) && (inside_body)) {
                                            break;
                                        }
                                    }
                                }

                                *tokenizer = tokenizer_copy;
                            }
                        }
                    }
                }

                if(member_cnt > 0) {
                    // Count the real number of members.
                    Int real_member_cnt = 0;
                    for(Int i = 0; (i < member_cnt); ++i) {
                        Int var_cnt = 1;
                        Char *at = member_info[i].pos;
                        while(*at != ';') {
                            if(*at == ',') {
                                ++var_cnt;
                            }

                            ++at;
                        }

                        real_member_cnt += var_cnt;
                    }

                    // Now actually parse the member variables.
                    res.sd.members = new Variable[real_member_cnt];
                    if(res.sd.members) {
                        Int member_index = 0;
                        for(Int i = 0; (i < member_cnt); ++i) {
                            Int var_cnt = 1;
                            Char *at = member_info[i].pos;
                            while(*at != ';') {
                                if(*at == ',') {
                                    ++var_cnt;
                                }

                                ++at;
                            }

                            for(Int j = 0; (j < var_cnt); ++j) {
                                Tokenizer fake_tokenizer = { member_info[i].pos };
                                res.sd.members[member_index] = parse_member(&fake_tokenizer, j);
                                res.sd.members[member_index].is_inside_anonymous_struct = member_info[i].is_inside_anonymous_struct;
                                res.sd.members[member_index].access = member_info[i].access;
                                ++member_index;
                            }
                        }

                        res.sd.member_count = member_index;
                    }
                }
            }

            delete[] member_info;
        }


        // Set the tokenizer to the end of the struct.
        eat_tokens(&start, 2);
        eat_tokens(&start, 3 * res.sd.inherited_count);
        skip_to_matching_bracket(&start);
        *tokenizer = start;
    }

    return(res);
}

typedef struct {
    Enum_Data ed;
    Bool success;
} ParseEnumResult;
ParseEnumResult parse_enum(Tokenizer *tokenizer) {
    ParseEnumResult res = {0};

    if(!is_forward_declared(tokenizer)) {
        Token name = get_token(tokenizer);
        Bool is_enum_struct = false;
        if((token_equals(name, "class")) || (token_equals(name, "struct"))) {
            is_enum_struct = true;
            name = get_token(tokenizer);
        }

        if(name.type == Token_Type_identifier) {
            // If the enum has an underlying type, get it.
            Token underlying_type = {0};
            Token next = get_token(tokenizer);
            if(next.type == Token_Type_colon) {
                underlying_type = get_token(tokenizer);
                next = get_token(tokenizer);
            }

            if(next.type == Token_Type_open_brace) {
                assert(name.type == Token_Type_identifier);
                assert((underlying_type.type == Token_Type_identifier) || (underlying_type.type == Token_Type_unknown));

                Token token = {0};

                res.ed.is_struct = is_enum_struct;

                // If I call token_to_string here, Visual Studio 2010 ends up generating code which uses MOVAPS on unaligned memory
                // and crashes. So that's why I didn't...
                res.ed.name.e = name.e; res.ed.name.len = name.len;

                if(underlying_type.type == Token_Type_identifier) {
                    res.ed.type = token_to_string(underlying_type);
                }

                Tokenizer copy = *tokenizer;
                res.ed.no_of_values = 1;
                token = get_token(&copy);
                while(token.type != Token_Type_close_brace) {
                    if(token.type == Token_Type_comma) {
                        Token tmp = get_token(&copy);

                        if(tmp.type == Token_Type_identifier) {
                            ++res.ed.no_of_values;
                        } else if(tmp.type == Token_Type_close_brace) {
                            break;
                        }
                    }

                    token = get_token(&copy);
                }

                res.ed.values = new Enum_Value[res.ed.no_of_values];
                if(res.ed.values) {
                    for(Int i = 0, count = 0; (i < res.ed.no_of_values); ++i, ++count) {
                        Enum_Value *ev = res.ed.values + i;

                        Token temp_token = {0};
                        while(temp_token.type != Token_Type_identifier) { temp_token = get_token(tokenizer); }

                        ev->name = token_to_string(temp_token);
                        if(peak_token(tokenizer).type == Token_Type_assign) {
                            eat_token(tokenizer);
                            Token num = get_token(tokenizer);

                            ResultInt r = token_to_int(num);
                            if(r.success) { count = r.e;                                }
                            else          { push_error(ErrorType_failed_to_parse_enum); }
                        }

                        ev->value = count;
                    }

                    res.success = true;
                }
            }
        }
    }

    return(res);
}

Token_Type get_token_type(String s) {
    assert(s.len);

    Token_Type res = Token_Type_unknown;
    switch(s.e[0]) {
        case 0:   { res = Token_Type_end_of_stream; } break;

        case '(': { res = Token_Type_open_paren;    } break;
        case ')': { res = Token_Type_close_paren;   } break;
        case ':': { res = Token_Type_colon;         } break;
        case ';': { res = Token_Type_semi_colon;    } break;
        case '*': { res = Token_Type_asterisk;      } break;
        case '[': { res = Token_Type_open_bracket;  } break;
        case ']': { res = Token_Type_close_bracket; } break;
        case '{': { res = Token_Type_open_brace;    } break;
        case '}': { res = Token_Type_close_brace;   } break;
        case ',': { res = Token_Type_comma;         } break;
        case '~': { res = Token_Type_tilde;         } break;
        case '#': { res = Token_Type_hash;          } break;
        case '&': { res = Token_Type_ampersand;     } break;
        case '+': { res = Token_Type_plus;          } break;
        case '-': { res = Token_Type_minus;         } break;
        case '/': { res = Token_Type_divide;        } break;
        case '|': { res = Token_Type_inclusive_or;  } break;

        case '=': {
            if(s.len == 1)                           { res = Token_Type_assign; }
            else if((s.len == 2) && (s.e[1] == '=')) { res = Token_Type_equal;  }
            else                                     { assert(0);              }
        } break;

        case '!': {
            if(s.len == 1)                           { res = Token_Type_not; }
            else if((s.len == 2) && (s.e[1] == '=')) { res = Token_Type_not_equal; }
        } break;

        case '>': {
            if(s.len == 1)                           { res = Token_Type_open_angle_bracket;    }
            else if((s.len == 2) && (s.e[1] == '=')) { res = Token_Type_greater_than_or_equal; }
            else                                     { assert(0);                             }
        } break;

        case '<': {
            if(s.len == 1)                           { res = Token_Type_open_angle_bracket; }
            else if((s.len == 2) && (s.e[1] == '=')) { res = Token_Type_less_than_or_equal; }
            else                                     { assert(0);                          }
        } break;

        default: {
            if(is_num(s.e[0])) { res = Token_Type_number;     }
            else               { res = Token_Type_identifier; }
        } break;
    }

    return(res);
}

Token string_to_token(String str) {
    Token res = {0};

    res.e = str.e;
    res.len = str.len;
    res.type = get_token_type(str);

    return(res);
}

Token get_token(Tokenizer *tokenizer) {
    eat_whitespace(tokenizer);

    Token res = {0};
    res.len = 1;
    res.e = tokenizer->at;
    Char c = tokenizer->at[0];
    ++tokenizer->at;

    switch(c) {
        case 0:   { res.type = Token_Type_end_of_stream; } break;

        case '(': { res.type = Token_Type_open_paren;    } break;
        case ')': { res.type = Token_Type_close_paren;   } break;
        case ':': { res.type = Token_Type_colon;         } break;
        case ';': { res.type = Token_Type_semi_colon;    } break;
        case '*': { res.type = Token_Type_asterisk;      } break;
        case '[': { res.type = Token_Type_open_bracket;  } break;
        case ']': { res.type = Token_Type_close_bracket; } break;
        case '{': { res.type = Token_Type_open_brace;    } break;
        case '}': { res.type = Token_Type_close_brace;   } break;
        case ',': { res.type = Token_Type_comma;         } break;
        case '~': { res.type = Token_Type_tilde;         } break;
        case '#': { res.type = Token_Type_hash;          } break;
        case '&': { res.type = Token_Type_ampersand;     } break;
        case '+': { res.type = Token_Type_plus;          } break;
        case '-': { res.type = Token_Type_minus;         } break;
        case '/': { res.type = Token_Type_divide;        } break;
        case '|': { res.type = Token_Type_inclusive_or;  } break;

        case '=': {
            Token next = peak_token(tokenizer);
            if(next.type == Token_Type_assign) { res.type = Token_Type_equal;  res.len = 2; }
            else                              { res.type = Token_Type_assign;              }
        } break;

        case '!': {
            Token next = peak_token(tokenizer);
            if(next.type == Token_Type_assign) { res.type = Token_Type_not_equal; res.len = 2; }
            else                              { res.type = Token_Type_not;                    }
        } break;

        case '>': {
            Token next = peak_token(tokenizer);
            if(next.type == Token_Type_assign) { res.type = Token_Type_greater_than_or_equal; res.len = 2; }
            else                              { res.type = Token_Type_close_angle_bracket;                }
        } break;

        case '<': {
            Token next = peak_token(tokenizer);
            if(next.type == Token_Type_assign) { res.type = Token_Type_less_than_or_equal; res.len = 2; }
            else                              { res.type = Token_Type_open_angle_bracket;              }
        } break;

        case '.':  {
            Bool var_args = false;
            Tokenizer tokenizer_copy = *tokenizer;
            Token next = get_token(&tokenizer_copy);
            if(next.type == Token_Type_period) {
                next = get_token(&tokenizer_copy);
                if(next.type == Token_Type_period) {
                    var_args = true;

                    res.type = Token_Type_var_args;
                    res.len = 3;

                    eat_tokens(tokenizer, 2);
                }
            }

            if(!var_args) { res.type = Token_Type_period; }
        } break;

        case '"': {
            res.e = tokenizer->at;
            while((tokenizer->at[0]) && (tokenizer->at[0] != '"')) {
                if((tokenizer->at[0] == '\\') && (tokenizer->at[1])) { ++tokenizer->at; }
                ++tokenizer->at;
            }

            res.type = Token_Type_string;
            res.len = safe_truncate_size_64(tokenizer->at - res.e);
            if(tokenizer->at[0] == '"') { ++tokenizer->at; }
        } break;

        case '\'': {
            res.e = tokenizer->at;
            while((tokenizer->at[0]) && (tokenizer->at[0] != '\'')) {
                if((tokenizer->at[0] == '\\') && (tokenizer->at[1])) { ++tokenizer->at; }
                ++tokenizer->at;
            }

            res.type = Token_Type_string;
            res.len = safe_truncate_size_64(tokenizer->at - res.e);
            if(tokenizer->at[0] == '\'') { ++tokenizer->at; }
        } break;

        default: {
            if((is_alphabetical(c)) || (c == '_')) {
                while((is_alphabetical(tokenizer->at[0])) || (is_num(tokenizer->at[0])) || (tokenizer->at[0] == '_')) {
                    ++tokenizer->at;
                }

                res.len = safe_truncate_size_64(tokenizer->at - res.e);
                res.type = Token_Type_identifier;
            } else if(is_num(c)) {
                while(is_num(tokenizer->at[0])) { ++tokenizer->at; }

                res.len = safe_truncate_size_64(tokenizer->at - res.e);
                res.type = Token_Type_number;
            } else {
                res.type = Token_Type_unknown;
            }
        } break;
    }

    return(res);
}

Bool is_linkage_token(Token token) {
    Bool res = token_equals(token, "static");
    if(!res) {
        res = token_equals(token, "inline");
    }

    return(res);
}

Bool is_ptr_or_ref(Token token) {
    if((token.type == Token_Type_ampersand) || (token.type == Token_Type_asterisk)) {
        return(true);
    } else {
        return(false);
    }
}

Bool is_control_keyword(Token token) {
    if(token_equals(token, "if")) {
        return(true);
    } else if(token_equals(token, "do")) {
        return(true);
    } else if(token_equals(token, "while")) {
        return(true);
    } else {
        return(false);
    }
}

typedef struct {
    Bool success;
    Function_Data fd;
} AttemptFunctionResult;
AttemptFunctionResult attempt_to_parse_function(Token token, Tokenizer *tokenizer) {
#if INTERNAL
    Tokenizer debug_copy_tokenizer = *tokenizer;
#endif

    AttemptFunctionResult res = {0};

    Token linkage = {0}, result = {0}, name = {0};
    Int return_pointer_cnt = 0; // TODO(Jonny): Support returning references.
    Bool return_ref = false;

    // Find out the linkage, if any.
    if(is_linkage_token(token)) {
        linkage = token;
        result = get_token(tokenizer);
    } else {
        result = token;
    }

    if(result.type == Token_Type_identifier) {
        // Get whether the result is a reference or pointer.
        Token peak = peak_token(tokenizer);
        while(is_ptr_or_ref(peak)) {
            eat_token(tokenizer);
            if(peak.type == Token_Type_ampersand) {
                return_ref = true;
            } else {
                ++return_pointer_cnt;
            }

            peak = peak_token(tokenizer);
        }



        if(!is_control_keyword(peak)) { // Skip if, do, and while loops.
            // Get function name.
            if((peak.type == Token_Type_identifier) && (!token_equals(peak, "operator"))) {
                name = peak;
                eat_token(tokenizer);

                Token ob = get_token(tokenizer);
                if(ob.type == Token_Type_open_paren) {
                    Variable *params = new Variable[8];
                    Int param_cnt = 0;

                    Token t = peak_token(tokenizer);

                    // Special code to handle void as only param.
                    if(token_equals(t, "void")) {
                        if(require_token(tokenizer, Token_Type_close_paren)) {
                            get_token(tokenizer);
                            t = get_token(tokenizer);
                        }
                    }

                    Bool successfully_parsed_members = true;
                    while((t.type != Token_Type_open_brace) && (t.type != Token_Type_close_paren)) {
                        if(t.type == Token_Type_var_args) {
                            // TODO(Jonny): Handle.
                        } else {
                            Parse_Variable_Result v = parse_variable(tokenizer, Token_Type_comma, Token_Type_close_paren);
                            if(v.success) {
                                params[param_cnt++] = v.var;
                            } else {
                                successfully_parsed_members = false;
                            }

                            t = get_token(tokenizer);
                        }
                    }

                    if(successfully_parsed_members) {
                        res.success = true;

                        res.fd.linkage = token_to_string(linkage);
                        res.fd.return_type = token_to_string(result);
                        res.fd.name = token_to_string(name);
                        res.fd.params = params;
                        res.fd.param_cnt = param_cnt;
                        res.fd.return_type_ptr = return_pointer_cnt;
                    } else {
                        delete[] params;
                    }
                }
            }
        }
    }

    return(res);
}

Bool is_token_storage_keyword(Token token) {
    Bool res = false;

    if(token_compare_cstring(token, "struct")) {
        res = true;
    } else if(token_compare_cstring(token, "enum")) {
        res = true;
    } else if(token_compare_cstring(token, "union")) {
        res = true;
    } else if(token_compare_cstring(token, "class")) {
        res = true;
    }

    return(res);
}

Void parse_stream(Char *stream, Parse_Result *res) {
    Tokenizer tokenizer = { stream };

    Bool parsing = true;
    while(parsing) {
        Token token = get_token(&tokenizer);
        switch(token.type) {
            case Token_Type_end_of_stream: {
                parsing = false;
            } break;

            case Token_Type_hash: {
                // TODO(Jonny): Assert false in here after I've written the macro parsing stage. Should be no #'s after that.
                skip_to_end_of_line(&tokenizer);
            } break;

            case Token_Type_identifier: {
                if((token_equals(token, "struct")) || (token_equals(token, "class")) || (token_equals(token, "union"))) {
                    Struct_Type struct_type = {};

                    if(token_equals(token, "struct"))     { struct_type = StructType_struct; }
                    else if(token_equals(token, "class")) { struct_type = StructType_class;  }
                    else if(token_equals(token, "union")) { struct_type = StructType_union;  }

                    if(res->structs.cnt + 1 >= res->struct_max) {
                        res->struct_max *= 2;
                        Void *p = system_realloc(res->structs.e, res->struct_max);
                        if(p) {
                            res->structs.e = (Struct_Data *)p;
                        }
                    }

                    Parse_Struct_Result r = parse_struct(&tokenizer, struct_type);

                    if(r.success) {
                        Bool already_added = false;
                        for(Int i = 0; (i < res->structs.cnt); ++i) {
                            Struct_Data *sd = res->structs.e + i;

                            if(string_comp(sd->name, r.sd.name)) {
                                already_added = true;
                                break;
                            }
                        }

                        if(!already_added) {
                            res->structs.e[res->structs.cnt++] = r.sd;
                        }
                    }
                } else if(token_equals(token, "enum")) {
                    if(res->enums.cnt + 1 >= res->enum_max) {
                        res->enum_max *= 2;
                        Void *p = system_realloc(res->enums.e, res->enum_max);
                        if(p) {
                            res->enums.e = (Enum_Data *)p;
                        }
                    }

                    ParseEnumResult r = parse_enum(&tokenizer);
                    if(r.success) {
                        res->enums.e[res->enums.cnt++] = r.ed;
                    }
                } else if(token_equals(token, "typedef")) {
                    Bool is_type = false, is_function = false;
                    // Check to see if this is a functon, type (struct) or not.
                    {
                        Tokenizer cpy = tokenizer;
                        Token t = {0};
                        do {
                            t = get_token(&cpy);
                            if(t.type == Token_Type_open_paren) {
                                is_function = true;
                                break;
                            } else if(is_token_storage_keyword(t)) {
                                is_type = true;
                                break;
                            }

                        } while(t.type != Token_Type_semi_colon);
                    }

                    if((!is_type) && (!is_function)) {
                        Token original = peak_token(&tokenizer);
                        if(original.type == Token_Type_identifier) {
                            eat_token(&tokenizer);

                            Token fresh = peak_token(&tokenizer);
                            if(original.type == Token_Type_identifier) {
                                eat_token(&tokenizer);

                                res->typedefs.e[res->typedefs.cnt].original = token_to_string(original);
                                res->typedefs.e[res->typedefs.cnt].fresh = token_to_string(fresh);

                                res->typedefs.cnt++;
                            }
                        }
                    } else if(is_function) {
                        Token name_before_paren = {0};
                        for(;;) {
                            Token tmp = get_token(&tokenizer);
                            if(tmp.type == Token_Type_open_paren) {
                                break;
                            }

                            name_before_paren = tmp;
                        }

                        Typedef_Data *td = res->typedefs.e + res->typedefs.cnt++;

                        td->original = create_string("uintptr_t");
                        td->fresh = token_to_string(name_before_paren);
                    }
                } else {
#if 0
                    AttemptFunctionResult r = attempt_to_parse_function(token, &tokenizer);
                    if(r.success) {
                        if(res->funcs.cnt + 1 >= res->func_max) {
                            res->func_max *= 2;
                            Void *p = system_realloc(res->funcs.e, res->func_max);
                            if(p) {
                                res->funcs.e = cast(Function_Data *)p;
                            }
                        }

                        // TODO(Jonny): Realloc if nessessary.
                        res->funcs.e[res->funcs.cnt++] = r.fd;
                    }
#endif
                }
            } break;
        }
    }
}

//
// Preprocessor.
//
typedef struct {
    String iden;
    String res;

    String *params;
    Int param_cnt;
} MacroData;

typedef struct {
    Char *e;
    Uintptr size;
    Uintptr memory_size;
} File_With_Extra_Space;

void move_bytes_forward(Void *ptr, Uintptr num_bytes_to_move, Uintptr move_pos) {
    Byte *ptr8 = cast(Byte *)ptr;
    for(Intptr i = num_bytes_to_move - 1; (i >= 0); --i) {
        ptr8[i + move_pos] = ptr8[i];
    }
}

void move_bytes_backwards(Void *ptr, Uintptr num_bytes_to_move, Uintptr move_pos) {
    Byte *ptr8 = cast(Byte *)ptr;
    for(Uintptr i = 0; (i < num_bytes_to_move); ++i) {
        ptr8[i + move_pos] = ptr8[i];
    }

    ptr8[num_bytes_to_move + move_pos] = 0;
}

Void move_stream(File_With_Extra_Space *file, Char *offset_ptr, Intptr amount_to_move) {
    Uintptr offset = offset_ptr - file->e;
    if(amount_to_move < 0) {
        Uintptr abs_amount_to_move = absolute_value((Int)amount_to_move);
        file->size -= abs_amount_to_move;

        for(Uintptr i = offset; (i < file->size); ++i) {
            file->e[i] = file->e[i + abs_amount_to_move];
        }

        file->e[file->size] = 0;
    } else {
        Uintptr old_size = file->size;
        Uintptr new_size = file->size + amount_to_move;

        // Zero the new bit.
        for(Uintptr i = old_size; (i < new_size); ++i) {
            file->e[i] = 0;
        }

        // Copy thw new bits.
        Uintptr tmp = old_size - offset;

        Char *end = file->e + amount_to_move + offset + tmp;
        Char *start = file->e + offset + tmp;
#if INTERNAL
        Char *end_ = end;
        Char *start_ = start;
#endif

        for(Uintptr i = old_size; (i > offset); --i) {
            *end-- = *start--;
        }

        file->size = new_size;
    }

    assert(file->e[0]);
}

typedef struct {
    MacroData md;
    Bool success;
} ParseMacroResult;
ParseMacroResult parse_macro(Tokenizer *tokenizer, TempMemory *param_memory) {
    ParseMacroResult res = {0};

    Token iden = get_token(tokenizer);

    while(is_whitespace_no_end_line(*tokenizer->at)) {
        ++tokenizer->at;
    }

    res.md.iden = token_to_string(iden);

    if(is_end_of_line(*tokenizer->at)) {
        res.success = true;
    } else {
        if(*tokenizer->at == '(') {
            res.md.params = cast(String *)(cast(Char *)param_memory->e + param_memory->used);

            Token param = {0};
            do {
                param = get_token(tokenizer);
                if(param.type == Token_Type_identifier) {
                    res.md.params[res.md.param_cnt++] = token_to_string(param);
                }
            } while(param.type != Token_Type_close_paren);

            param_memory->used += sizeof(String) * res.md.param_cnt;
        }

        String macro_res = {0};
        macro_res.e = tokenizer->at;
        while(!is_end_of_line(*tokenizer->at)) {
            ++tokenizer->at;
            ++macro_res.len;
        }

        res.md.res = macro_res;
        res.success = true;
    }

    return(res);
}

Void macro_replace(Char *token_start, File_With_Extra_Space *file, MacroData md) {
    TempMemory tm = create_temp_buffer(megabytes(1)); // TODO(Jonny): Arbitrary Size.

    Tokenizer tokenizer = { token_start };

    Uintptr iden_len = md.iden.len;
    String *params = 0;

    // Read the parameters the user passed in.
    if(md.param_cnt) {
        params = cast(String *)push_size(&tm, sizeof(String) * md.param_cnt);

        Int brace_cnt = 0; // Is used for when we pass a function into a macro, so the commas don't confuse things.

        Tokenizer cpy = tokenizer;
        cpy.at += md.iden.len + 1; // Skip identifier and open parenthesis.

        String *p = params;
        Int max_len = 256;
        p->e = cast(Char *)push_size(&tm, sizeof(Char) * max_len);
        do {
top:;
            if(*cpy.at == '(') {
                ++brace_cnt;
            }

            ++iden_len;
            if((*cpy.at == ',') && (!brace_cnt)) {
                ++p;
                p->e = cast(Char *)push_size(&tm, sizeof(Char) * max_len);
            } else {
                p->e[p->len++] = *cpy.at;
                assert(p->len < max_len);

                if((brace_cnt) && (*cpy.at == ')')) {
                    --brace_cnt;
                    goto top;
                }
            }

            ++cpy.at;
        } while((*cpy.at != ')') || (brace_cnt));

        iden_len += 2;
    }

    Char *end = tokenizer.at + iden_len - absolute_value((Int)(md.res.len - iden_len));

    move_stream(file, token_start, md.res.len - iden_len);
    copy(token_start, md.res.e, md.res.len);

    // Replace macro parameters.
    if(md.param_cnt) {
        Token token = get_token(&tokenizer);

        do {
            for(Int i = 0; (i < md.param_cnt); ++i) {
                assert(file->e[0]);
                if(token_compare_string(token, md.params[i])) {
                    // TODO(Jonny): After the breakpoint in the if is hit, the paramters for md are invalidated after move_stream.
                    //              I check the memory and that seems to be fine, it's just the actual pointer that's getting fucked...
                    //              somehow.
                    move_stream(file, tokenizer.at - 1, params[i].len - token.len);
                    assert(file->size < file->memory_size);

                    iden_len += params[i].len - token.len;
                    end += params[i].len - token.len;
                    for(Int j = 0; (j < params[i].len); ++j) {
                        token.e[j] = params[i].e[j];
                    }
                }
            }

            token = get_token(&tokenizer);

        } while(tokenizer.at <= end);
    }

    free_temp_buffer(&tm);
}

Void add_include_file(Tokenizer *tokenizer, File_With_Extra_Space *file) {
    eat_whitespace(tokenizer);

    // Get a copy of the name.
    Char name_buf[256] = {0};
    Char *at = name_buf;

    ++tokenizer->at;
    while((*tokenizer->at != '"') && (*tokenizer->at != '>')) {
        *at++ = *tokenizer->at++;
    }

    ++tokenizer->at;

    if(!string_comp(name_buf, "pp_generated.h")) {
        File include_file = system_read_entire_file_and_null_terminate(name_buf);

        if(include_file.size) {
            // Tokenizer is invalid after the move_stream call.
            Uintptr offset = tokenizer->at - file->e;
            move_stream(file, tokenizer->at, include_file.size);
            tokenizer->at = file->e + offset;

            copy(tokenizer->at, include_file.e, include_file.size);

            system_free(include_file.e);
        }
    }
}

File preprocess_macros(File original_file) {
    File res = {0};

    File_With_Extra_Space file = {};
    file.size = original_file.size;
    file.memory_size = original_file.size * 10;

    Void *p = system_malloc(file.memory_size);
    if(p) {
        copy(p, original_file.e, original_file.size);
        file.e = (Char *)p;
        zero(original_file.e, original_file.size);
        system_free(original_file.e);

        TempMemory param_memory = create_temp_buffer(sizeof(String) * 128);

        Int macro_cnt = 0, macro_max = 128;
        MacroData *macro_data = new MacroData[macro_max];

        Tokenizer tokenizer = { file.e };

        Bool parsing = true;
        Token prev_token = {0};
        while(parsing) {
            Token token = get_token(&tokenizer);
            switch(token.type) {
                case Token_Type_hash: {
                    Token preprocessor_dir = get_token(&tokenizer);

                    if(token_compare_cstring(preprocessor_dir, "include")) { // #include files.
                        add_include_file(&tokenizer, &file);
                    } else if(token_compare_cstring(preprocessor_dir, "define")) { // #define
                        ParseMacroResult pmr = parse_macro(&tokenizer, &param_memory);
                        if(pmr.success) {
                            assert(macro_cnt + 1 < macro_max);
                            macro_data[macro_cnt++] = pmr.md;
                        }
                    } else if(token_compare_cstring(preprocessor_dir, "undef")) { // #undef
                        // TODO(Jonny): Hacky as fuck...
                        Token undef_macro = get_token(&tokenizer);
                        for(Int i = 0; (i < macro_cnt); ++i) {
                            if(token_compare_string(undef_macro, macro_data[i].iden)) {
                                for(Int j = 0; (j < macro_data[i].iden.len); ++j) {
                                    macro_data[i].iden.e[j] = ' ';
                                }
                            }
                        }
                    } else if(preprocessor_dir.type == Token_Type_hash) {
                        Char *prev_token_end = prev_token.e + prev_token.len;
                        Token next_token = peak_token(&tokenizer);
                        Intptr diff = next_token.e - prev_token_end;

                        move_stream(&file, token.e, -diff);
                    }
                } break;

                case Token_Type_identifier: {
                    if(token_compare_cstring(token, "SGLM_MADD")) {
                        int i = 0;
                    }

                    for(Int i = 0; (i < macro_cnt); ++i) {
                        if(token_compare_string(token, macro_data[i].iden)) {
                            if(macro_data[i].res.len) {
                                Char *start = token.e;
                                macro_replace(token.e, &file, macro_data[i]);
                                tokenizer.at = start;
                            }
                        }
                    }
                } break;

                case Token_Type_end_of_stream: {
                    parsing = false;
                };
            }

            prev_token = token;
        }

        free_temp_buffer(&param_memory);
        delete[] macro_data;

        assert(file.size < file.memory_size);

        res.e = file.e;
        res.size = file.size;
    }

    return(res);
}

//
// Start point.
//
Parse_Result parse_streams(Int cnt, Char **fnames) {
    Parse_Result res = {0};

    res.enum_max = 8;
    res.enums.e = new Enum_Data[res.enum_max];

    res.struct_max = 32;
    res.structs.e = new Struct_Data[res.struct_max];

    res.func_max = 128;
    res.funcs.e = new Function_Data[res.func_max];

    res.typedef_max = 128;
    res.typedefs.e = new Typedef_Data[res.typedef_max];

    for(Int i = 0; (i < cnt); ++i) {
        File file = system_read_entire_file_and_null_terminate(fnames[i]); // TODO(Jonny): Leak.
        if(file.size) {
            file = preprocess_macros(file);
            parse_stream(file.e, &res);
        } else {
            system_write_to_console("Mirror error: Cannot find file %s", fnames[i]);
        }
    }

    return(res);
}
