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

#include "lexer.h"
#include "platform.h"

struct MacroData {
    String iden;
    String res;
};
internal MacroData *macro_data = 0;
internal Int macro_count = 0;

// TODO(Jonny): Rename some of these so they're more clear.
enum TokenType {
    TokenType_unknown,

    TokenType_open_paren,
    TokenType_close_paren,
    TokenType_colon,
    TokenType_semi_colon,
    TokenType_asterisk,
    TokenType_open_bracket,
    TokenType_close_bracket,
    TokenType_open_brace,
    TokenType_close_brace,
    TokenType_open_angle_bracket,
    TokenType_close_angle_bracket,
    TokenType_hash,
    TokenType_assign,
    TokenType_comma,
    TokenType_tilde,
    TokenType_period,
    TokenType_ampersand,
    TokenType_inclusive_or,
    TokenType_not,
    TokenType_var_args,

    TokenType_plus,
    TokenType_minus,
    TokenType_divide,

    TokenType_number,
    TokenType_identifier,
    TokenType_string,

    TokenType_equal,
    TokenType_not_equal,
    TokenType_greater_than_or_equal,
    TokenType_less_than_or_equal,

    TokenType_error,

    TokenType_end_of_stream,
};

struct Token {
    Char *e;
    Int len;

    TokenType type;
};

struct Tokenizer {
    Char *at;
    Int line;
};

// TODO(Jonny): Line numbers won't match now, remove tokenizer.
internal Bool is_end_of_line(Char c, Tokenizer *tokenizer = 0) {
    Bool res = ((c == '\n') || (c == '\r'));
    if(c == '\n') {
        if(tokenizer) {
            ++tokenizer->line;
        }
    }

    return(res);
}

Bool is_whitespace(Char c) {
    Bool res = ((c == ' ') || (c == '\t') || (c == '\v') || (c == '\f') || (is_end_of_line(c)));

    return(res);
}

internal Bool is_whitespace(Char c, Tokenizer *tokenizer) {
    Bool res = ((c == ' ') || (c == '\t') || (c == '\v') || (c == '\f') || (is_end_of_line(c, tokenizer)));

    return(res);
}

internal Void skip_to_end_of_line(Tokenizer *tokenizer) {
    while(is_end_of_line(*tokenizer->at, tokenizer)) {
        ++tokenizer->at;
    }
}

internal String token_to_string(Token token) {
    String res = { token.e, token.len };

    return(res);
}

internal Void token_to_string(Token token, Char *buf, Int size) {
    assert(size > token.len);
    for(Int i = 0; (i < token.len); ++i, ++buf) {
        *buf = token.e[i];
    }

    *buf = 0;
}

internal Bool token_compare(Token a, Token b) {
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

internal ResultInt token_to_int(Token t) {
    String str = token_to_string(t);
    ResultInt res = string_to_int(str);

    return(res);
}

// TODO(Jonny): Create a token_equals_keyword function. This could also test macro'd aliases for keywords,
//              as well as the actual keyword.

internal Bool token_equals(Token token, Char *str) {
    Bool res = false;

    Int len = string_length(str);
    if(len == token.len) {
        res = string_comp_len(token.e, str, len);
    }

    return(res);
}

internal Token get_token(Tokenizer *tokenizer); // Because C/C++...
internal Token peak_token(Tokenizer *tokenizer) {
    Tokenizer cpy = *tokenizer;
    Token res = get_token(&cpy);

    return(res);
}

internal Void loop_until_token(Tokenizer *tokenizer, TokenType type) {
    Token token = get_token(tokenizer);
    while(token.type != type) {
        token = get_token(tokenizer);
    }
}

internal Variable parse_member(Tokenizer *tokenizer, Int var_to_parse) {
    Variable res = {};
    res.array_count = 1;

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

    res.type = token_to_string(type);

    if(var_to_parse) {
        Char *at = tokenizer->at;
        int cur = 0;
        while(cur < var_to_parse) {
            Token temp = get_token(tokenizer);
            if(temp.type == TokenType_comma) {
                ++cur;
            }
        }
    }

    Bool parsing = true;
    while(parsing) {
        Token token = get_token(tokenizer);
        switch(token.type) {
            case TokenType_semi_colon: case TokenType_comma: case TokenType_end_of_stream: {
                parsing = false;
            } break;

            case TokenType_asterisk: {
                ++res.ptr;
            } break;

            case TokenType_identifier: {
                res.name = token_to_string(token);
            } break;

            case TokenType_open_bracket: {
                Token size_token = get_token(tokenizer);
                if(size_token.type == TokenType_number) {
                    char buf[32] = {};

                    token_to_string(size_token, buf, array_count(buf));
                    ResultInt arr_count = string_to_int(buf);
                    if(arr_count.success) {
                        res.array_count = arr_count.e;
                    } else {
                        push_error(ErrorType_failed_to_find_size_of_array);
                    }
                } else {
                    // TODO(Jonny): Something _bad_ happened...
                }
            } break;
        }
    }

    return(res);
}

internal Void eat_whitespace(Tokenizer *tokenizer) {
    for(;;) {
        if(!tokenizer->at[0]) { // Nul terminator.
            break;
        } else if(is_whitespace(tokenizer->at[0], tokenizer)) { // Whitespace
            ++tokenizer->at;
        } else if((tokenizer->at[0] == '/') && (tokenizer->at[1] == '/')) { // C++ comments.
            tokenizer->at += 2;
            while((tokenizer->at[0]) && (!is_end_of_line(tokenizer->at[0], tokenizer))) {
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
                    if(tokenizer->at[0] == '\n') {
                        ++tokenizer->line;
                    } else if(tokenizer->at[0] == '#') {
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
                    if(tokenizer->at[0] == '\n') {
                        ++tokenizer->line;
                    }
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

internal Bool is_alphabetical(Char c) {
    Bool res = (((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')));

    return(res);
}

internal Bool is_num(Char c) {
    Bool res = ((c >= '0') && (c <= '9'));

    return(res);
}

internal Void parse_number(Tokenizer *tokenizer) {
    // TODO(Jonny): Implement.
}

#define eat_token(tokenizer) eat_tokens(tokenizer, 1)
internal Void eat_tokens(Tokenizer *tokenizer, Int num_tokens_to_eat) {
    for(Int i = 0; (i < num_tokens_to_eat); ++i) {
        get_token(tokenizer);
    }
}

internal Bool require_token(Tokenizer *tokenizer, TokenType desired_type) {
    Token token = get_token(tokenizer);
    Bool res = (token.type == desired_type);

    return(res);
}

internal Bool peak_require_token(Tokenizer *tokenizer, Char *str) {
    Bool res = false;
    Tokenizer cpy = *tokenizer;
    Token token = get_token(&cpy);

    if(string_comp_len(token.e, str, token.len)) {
        res = true;
    }

    return(res);
}

internal Bool is_stupid_class_keyword(Token t) {
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

internal Void skip_to_matching_bracket(Tokenizer *tokenizer) {
    Int brace_count = 1;
    Token token = {};
    Bool should_loop = true;
    while(should_loop) {
        token = get_token(tokenizer);
        switch(token.type) {
            case TokenType_open_brace: ++brace_count; break;

            case TokenType_close_brace: {
                --brace_count;
                if(!brace_count) {
                    should_loop = false;
                }
            } break;
        }
    }
}

internal String parse_template(Tokenizer *tokenizer) {
    String res = {};

    res.e = tokenizer->at;
    eat_token(tokenizer);
    Int angle_bracket_count = 1;
    Token token;
    Bool should_loop = true;
    while(should_loop) {
        token = get_token(tokenizer);
        switch(token.type) {
            case TokenType_open_angle_bracket: {
                ++angle_bracket_count;
            } break;

            case TokenType_close_angle_bracket: {
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

struct ParseVariableRes {
    Variable var;
    Bool success;
};

internal ParseVariableRes parse_variable(Tokenizer *tokenizer, TokenType end_token_type_1,
                                         TokenType end_token_type_2 = TokenType_unknown) {
    ParseVariableRes res = {};

    // Return type.
    Token type = get_token(tokenizer);
    if(type.type == TokenType_identifier) {
        res.var.type = token_to_string(type);

        // Is pointer?
        Token token = get_token(tokenizer);
        while(token.type == TokenType_asterisk) {
            ++res.var.ptr;
            token = get_token(tokenizer);
        }

        // Name.
        if((token.len) && (token.type == TokenType_identifier)) {
            res.var.name = token_to_string(token);

            // Is array?
            token = peak_token(tokenizer);
            if((token.type != end_token_type_1) && (token.type != end_token_type_2)) {
                eat_token(tokenizer);
                if(token.type != TokenType_open_bracket) { push_error(ErrorType_failed_parsing_variable); }
                else {
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
            if(token.type == TokenType_assign) {
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
    Token token = {};
    Bool loop = true;
    while(loop) {
        token = get_token(&cpy);
        switch(token.type) {
            case TokenType_open_brace: {
                loop = false;
            } break;

            case TokenType_semi_colon: {
                loop = false;
                res = true;
            } break;

            case TokenType_end_of_stream: {
                loop = false;
            } break;
        }
    }

    return(res);
}

struct ParseStructResult {
    StructData sd;
    Bool success;
};
internal ParseStructResult parse_struct(Tokenizer *tokenizer, StructType struct_type, String template_header = create_string("") ) {
    ParseStructResult res = {};

    if(!is_forward_declared(tokenizer)) {
        if(template_header.len) {
            res.sd.template_header = template_header;
        }

        Tokenizer start = *tokenizer;

        Access current_access = ((struct_type == StructType_struct) || (struct_type == StructType_union)) ? Access_public : Access_private;
        res.sd.struct_type = struct_type;

        Bool have_name = false;
        Token name = peak_token(tokenizer);
        if(name.type == TokenType_identifier) {
            have_name = true;
            res.sd.name = token_to_string(name);
            eat_token(tokenizer);
        }

        Token peaked_token = peak_token(tokenizer);
        if(peaked_token.type == TokenType_colon) {
            res.sd.inherited = system_alloc(String, 8);

            eat_token(tokenizer);

            Token next = get_token(tokenizer);
            while(next.type != TokenType_open_brace) {
                if(!(is_stupid_class_keyword(next)) && (next.type != TokenType_comma)) {
                    // TODO(Jonny): Fix properly.
                    /*if(res.sd.inherited_count + 1 >= cast(Int)(get_alloc_size(res.sd.inherited) / sizeof(String))) {
                        Void *p = system_realloc(res.sd.inherited);
                        if(p) { res.sd.inherited = cast(String *)p; }
                    }*/

                    res.sd.inherited[res.sd.inherited_count++] = token_to_string(next);
                }

                next = peak_token(tokenizer);
                if(next.type != TokenType_open_brace) { eat_token(tokenizer); }
            }
        }

        if(require_token(tokenizer, TokenType_open_brace)) {
            res.success = true;

            res.sd.member_count = 0;
            Int member_cnt = 0;

            struct MemberInfo {
                Char *pos;
                Access access;
                Bool is_inside_anonymous_struct;
            };

            PtrSize member_info_mem_cnt = 256;
            MemberInfo *member_info = system_alloc(MemberInfo, member_info_mem_cnt);
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

                        if((token.type != TokenType_colon) && (token.type != TokenType_tilde)) {
                            if(token.type == TokenType_close_brace) {
                                if(inside_anonymous_struct) {
                                    inside_anonymous_struct = false;
                                    eat_token(tokenizer); // Eat semi colon.
                                    continue;
                                } else if(!have_name) {
                                    name = get_token(tokenizer);
                                    if(name.type == TokenType_identifier) res.sd.name = token_to_string(name);
                                    else                                  push_error(ErrorType_could_not_detect_struct_name);
                                }

                                break; // for
                            } else if(token.type == TokenType_hash) {
                                // TODO(Jonny): Support macros with '/' to extend their lines?
                                while(!is_end_of_line(tokenizer->at[0], tokenizer)) {
                                    ++tokenizer->at;
                                }
                            } else {
                                Bool is_func = false;

                                Tokenizer tokenizer_copy = *tokenizer;
                                Token temp = get_token(&tokenizer_copy);
                                while(temp.type != TokenType_semi_colon) {

                                    // Handle C++11 assignment within struct.
                                    if(temp.type == TokenType_assign) {
                                        loop_until_token(&tokenizer_copy, TokenType_semi_colon);

                                        break;
                                    }

                                    // Check if it is a function.
                                    if(temp.type == TokenType_open_paren) {
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
                                        if((t.type == TokenType_semi_colon) && (!inside_body)) {
                                            break;
                                        } else if(t.type == TokenType_open_brace) {
                                            inside_body = true;
                                        } else if((t.type == TokenType_close_brace) && (inside_body)) {
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
                    res.sd.members = system_alloc(Variable, real_member_cnt);
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

            system_free(member_info);
        }


        // Set the tokenizer to the end of the struct.
        eat_tokens(&start, 2);
        eat_tokens(&start, 3 * res.sd.inherited_count);
        skip_to_matching_bracket(&start);
        *tokenizer = start;
    }

    return(res);
}

struct ParseEnumResult {
    EnumData ed;
    Bool success;
};
internal ParseEnumResult parse_enum(Tokenizer *tokenizer) {
    ParseEnumResult res = {};

    if(!is_forward_declared(tokenizer)) {
        Token name = get_token(tokenizer);
        Bool is_enum_struct = false;
        if((token_equals(name, "class")) || (token_equals(name, "struct"))) {
            is_enum_struct = true;
            name = get_token(tokenizer);
        }

        if(name.type == TokenType_identifier) {
            // If the enum has an underlying type, get it.
            Token underlying_type = {};
            Token next = get_token(tokenizer);
            if(next.type == TokenType_colon) {
                underlying_type = get_token(tokenizer);
                next = get_token(tokenizer);
            }

            if(next.type == TokenType_open_brace) {
                //res = add_token_to_enum(name, underlying_type, is_enum_struct, &tokenizer);
                assert(name.type == TokenType_identifier);
                assert((underlying_type.type == TokenType_identifier) || (underlying_type.type == TokenType_unknown));

                Token token = {};

                res.ed.is_struct = is_enum_struct;

                // If I call token_to_string here, Visual Studio 2010 ends up generating code which uses MOVAPS on unaligned memory
                // and crashes. So that's why I didn't...
                res.ed.name.e = name.e; res.ed.name.len = name.len;

                if(underlying_type.type == TokenType_identifier) {
                    res.ed.type = token_to_string(underlying_type);
                }

                Tokenizer copy = *tokenizer;
                res.ed.no_of_values = 1;
                token = get_token(&copy);
                while(token.type != TokenType_close_brace) {
                    // TODO(Jonny): It was stupid to count the number of commas. Instead, actually count
                    //              the number of enums.
                    if(token.type == TokenType_comma) {
                        Token tmp = get_token(&copy);

                        if(tmp.type == TokenType_identifier) {
                            ++res.ed.no_of_values;
                        } else if(tmp.type == TokenType_close_brace) {
                            break;
                        }
                    }

                    token = get_token(&copy);
                }

                res.ed.values = system_alloc(EnumValue, res.ed.no_of_values);
                if(res.ed.values) {
                    for(Int i = 0, count = 0; (i < res.ed.no_of_values); ++i, ++count) {
                        EnumValue *ev = res.ed.values + i;

                        Token temp_token = {};
                        while(temp_token.type != TokenType_identifier) { temp_token = get_token(tokenizer); }

                        ev->name = token_to_string(temp_token);
                        if(peak_token(tokenizer).type == TokenType_assign) {
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

internal TokenType get_token_type(String s) {
    assert(s.len);

    TokenType res = TokenType_unknown;
    switch(s.e[0]) {
        case 0:   { res = TokenType_end_of_stream; } break;

        case '(': { res = TokenType_open_paren;    } break;
        case ')': { res = TokenType_close_paren;   } break;
        case ':': { res = TokenType_colon;         } break;
        case ';': { res = TokenType_semi_colon;    } break;
        case '*': { res = TokenType_asterisk;      } break;
        case '[': { res = TokenType_open_bracket;  } break;
        case ']': { res = TokenType_close_bracket; } break;
        case '{': { res = TokenType_open_brace;    } break;
        case '}': { res = TokenType_close_brace;   } break;
        case ',': { res = TokenType_comma;         } break;
        case '~': { res = TokenType_tilde;         } break;
        case '#': { res = TokenType_hash;          } break;
        case '&': { res = TokenType_ampersand;     } break;
        case '+': { res = TokenType_plus;          } break;
        case '-': { res = TokenType_minus;         } break;
        case '/': { res = TokenType_divide;        } break;
        case '|': { res = TokenType_inclusive_or;  } break;

        case '=': {
            if(s.len == 1)                           { res = TokenType_assign; }
            else if((s.len == 2) && (s.e[1] == '=')) { res = TokenType_equal;  }
            else                                     { assert(0);              }
        } break;

        case '!': {
            if(s.len == 1)                           { res = TokenType_not; }
            else if((s.len == 2) && (s.e[1] == '=')) { res = TokenType_not_equal; }
        } break;

        case '>': {
            if(s.len == 1)                           { res = TokenType_open_angle_bracket;    }
            else if((s.len == 2) && (s.e[1] == '=')) { res = TokenType_greater_than_or_equal; }
            else                                     { assert(0);                             }
        } break;

        case '<': {
            if(s.len == 1)                           { res = TokenType_open_angle_bracket; }
            else if((s.len == 2) && (s.e[1] == '=')) { res = TokenType_less_than_or_equal; }
            else                                     { assert(0);                          }
        } break;

        default: {
            if(is_num(s.e[0])) { res = TokenType_number;     }
            else               { res = TokenType_identifier; }
        } break;
    }

    return(res);
}

internal Token string_to_token(String str) {
    Token res = {};

    res.e = str.e;
    res.len = str.len;
    res.type = get_token_type(str);

    return(res);
}

internal Token get_token(Tokenizer *tokenizer) {
    eat_whitespace(tokenizer);

    Token res = {};
    res.len = 1;
    res.e = tokenizer->at;
    Char c = tokenizer->at[0];
    ++tokenizer->at;

    switch(c) {
        case 0:   { res.type = TokenType_end_of_stream; } break;

        case '(': { res.type = TokenType_open_paren;    } break;
        case ')': { res.type = TokenType_close_paren;   } break;
        case ':': { res.type = TokenType_colon;         } break;
        case ';': { res.type = TokenType_semi_colon;    } break;
        case '*': { res.type = TokenType_asterisk;      } break;
        case '[': { res.type = TokenType_open_bracket;  } break;
        case ']': { res.type = TokenType_close_bracket; } break;
        case '{': { res.type = TokenType_open_brace;    } break;
        case '}': { res.type = TokenType_close_brace;   } break;
        case ',': { res.type = TokenType_comma;         } break;
        case '~': { res.type = TokenType_tilde;         } break;
        case '#': { res.type = TokenType_hash;          } break;
        case '&': { res.type = TokenType_ampersand;     } break;
        case '+': { res.type = TokenType_plus;          } break;
        case '-': { res.type = TokenType_minus;         } break;
        case '/': { res.type = TokenType_divide;        } break;
        case '|': { res.type = TokenType_inclusive_or;  } break;

        case '=': {
            Token next = peak_token(tokenizer);
            if(next.type == TokenType_assign) { res.type = TokenType_equal;  res.len = 2; }
            else                              { res.type = TokenType_assign;              }
        } break;

        case '!': {
            Token next = peak_token(tokenizer);
            if(next.type == TokenType_assign) { res.type = TokenType_not_equal; res.len = 2; }
            else                              { res.type = TokenType_not;                    }
        } break;

        case '>': {
            Token next = peak_token(tokenizer);
            if(next.type == TokenType_assign) { res.type = TokenType_greater_than_or_equal; res.len = 2; }
            else                              { res.type = TokenType_close_angle_bracket;                }
        } break;

        case '<': {
            Token next = peak_token(tokenizer);
            if(next.type == TokenType_assign) { res.type = TokenType_less_than_or_equal; res.len = 2; }
            else                              { res.type = TokenType_open_angle_bracket;              }
        } break;

        case '.':  {
            Bool var_args = false;
            Tokenizer tokenizer_copy = *tokenizer;
            Token next = get_token(&tokenizer_copy);
            if(next.type == TokenType_period) {
                next = get_token(&tokenizer_copy);
                if(next.type == TokenType_period) {
                    var_args = true;

                    res.type = TokenType_var_args;
                    res.len = 3;

                    eat_tokens(tokenizer, 2);
                }
            }

            if(!var_args) { res.type = TokenType_period; }
        } break;

        case '"': {
            res.e = tokenizer->at;
            while((tokenizer->at[0]) && (tokenizer->at[0] != '"')) {
                if((tokenizer->at[0] == '\\') && (tokenizer->at[1])) { ++tokenizer->at; }
                ++tokenizer->at;
            }

            res.type = TokenType_string;
            res.len = safe_truncate_size_64(tokenizer->at - res.e);
            if(tokenizer->at[0] == '"') { ++tokenizer->at; }
        } break;

        case '\'': {
            res.e = tokenizer->at;
            while((tokenizer->at[0]) && (tokenizer->at[0] != '\'')) {
                if((tokenizer->at[0] == '\\') && (tokenizer->at[1])) { ++tokenizer->at; }
                ++tokenizer->at;
            }

            res.type = TokenType_string;
            res.len = safe_truncate_size_64(tokenizer->at - res.e);
            if(tokenizer->at[0] == '\'') { ++tokenizer->at; }
        } break;

        default: {
            if((is_alphabetical(c)) || (c == '_')) {
                while((is_alphabetical(tokenizer->at[0])) || (is_num(tokenizer->at[0])) || (tokenizer->at[0] == '_')) {
                    ++tokenizer->at;
                }

                res.len = safe_truncate_size_64(tokenizer->at - res.e);
                res.type = TokenType_identifier;
            } else if(is_num(c)) {
                while(is_num(tokenizer->at[0])) { ++tokenizer->at; }

                res.len = safe_truncate_size_64(tokenizer->at - res.e);
                res.type = TokenType_number;
            } else {
                res.type = TokenType_unknown;
            }
        } break;
    }

    if(res.type == TokenType_identifier) {
        Bool changed = false;
        do {
            changed = false;
            for(Int i = 0; (i < macro_count); ++i) {
                MacroData *md = macro_data + i;

                String token_as_string = token_to_string(res);
                if(string_comp(token_as_string, md->iden)) {
                    res = string_to_token(md->res);
                    changed = true;
                }
            }
        } while(changed);
    }

    //if(res.type == TokenType_unknown) { push_error(ErrorType_unknown_token_found); }

    return(res);
}

internal Bool is_linkage_token(Token token) {
    Bool res = token_equals(token, "static");
    if(!res) {
        res = token_equals(token, "inline");
    }

    return(res);
}

internal Bool is_ptr_or_ref(Token token) {
    if((token.type == TokenType_ampersand) || (token.type == TokenType_asterisk)) {
        return(true);
    } else {
        return(false);
    }
}

internal Bool is_control_keyword(Token token) {
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

struct AttemptFunctionResult {
    Bool success;
    FunctionData fd;
};

internal AttemptFunctionResult attempt_to_parse_function(Token token, Tokenizer *tokenizer) {
#if INTERNAL
    Tokenizer debug_copy_tokenizer = *tokenizer;
#endif

    AttemptFunctionResult res = {};

    Token linkage = {}, result = {}, name = {};
    Int return_pointer_cnt = 0; // TODO(Jonny): Support returning references.
    Bool return_ref = false;

    // Find out the linkage, if any.
    if(is_linkage_token(token)) {
        linkage = token;
        result = get_token(tokenizer);
    } else {
        result = token;
    }

    if(result.type == TokenType_identifier) {
        // Get whether the result is a reference or pointer.
        Token peak = peak_token(tokenizer);
        while(is_ptr_or_ref(peak)) {
            eat_token(tokenizer);
            if(peak.type == TokenType_ampersand) {
                return_ref = true;
            } else {
                ++return_pointer_cnt;
            }

            peak = peak_token(tokenizer);
        }



        if(!is_control_keyword(peak)) { // Skip if, do, and while loops.
            // Get function name.
            if((peak.type == TokenType_identifier) && (!token_equals(peak, "operator"))) {
                name = peak;
                eat_token(tokenizer);

                Token ob = get_token(tokenizer);
                if(ob.type == TokenType_open_paren) {
                    Variable *params = system_alloc(Variable, 8);
                    Int param_cnt = 0;

                    Token t = peak_token(tokenizer);

                    // Special code to handle void as only param.
                    if(token_equals(t, "void")) {
                        if(require_token(tokenizer, TokenType_close_paren)) {
                            get_token(tokenizer);
                            t = get_token(tokenizer);
                        }
                    }

                    Bool successfully_parsed_members = true;
                    while((t.type != TokenType_open_brace) && (t.type != TokenType_close_paren)) {
                        if(t.type == TokenType_var_args) {
                            // TODO(Jonny): Handle.
                        } else {
                            ParseVariableRes v = parse_variable(tokenizer, TokenType_comma, TokenType_close_paren);
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
                        system_free(params);
                    }
                }
            }
        }
    }

    return(res);
}

ParseResult parse_stream(Char *stream) {
    ParseResult res = {};

    Int enum_max = 8;
    res.enums.e = system_alloc(EnumData, enum_max);

    Int struct_max = 32;
    res.structs.e = system_alloc(StructData, struct_max);

    Int func_max = 128;
    res.funcs.e = system_alloc(FunctionData, func_max);

    Int macro_max = 32;
    macro_data = system_alloc(MacroData, macro_max);

    Int typedef_max = 64;
    res.typedefs.e = system_alloc(TypedefData, typedef_max);

    if((res.enums.e)  && (res.structs.e) && (res.funcs.e) && (macro_data) && (res.typedefs.e)) {
        Tokenizer tokenizer = { stream, 1 };

        String template_header = {};
        Bool parsing = true;
        while(parsing) {
            Token token = get_token(&tokenizer);
            switch(token.type) {
                case TokenType_end_of_stream: {
                    parsing = false;
                } break;

                case TokenType_hash: {
                    if(peak_require_token(&tokenizer, "define")) {
                        eat_token(&tokenizer);

                        MacroData *md = macro_data + macro_count++;
                        zero(md, sizeof(*md));

                        md->iden = token_to_string(get_token(&tokenizer));
                        eat_whitespace(&tokenizer);
                        md->res.e = tokenizer.at;
                        while(!is_end_of_line(*tokenizer.at, &tokenizer)) {
                            ++md->res.len;
                            ++tokenizer.at;
                        }
                    } else {
                        skip_to_end_of_line(&tokenizer);
                    }
                } break;

                case TokenType_identifier: {
                    // TODO(Jonny): I may need to keep the template header, so that the generated structs still work.
                    if(token_equals(token, "template")) {
                        template_header = parse_template(&tokenizer);
                    } else if((token_equals(token, "struct")) || (token_equals(token, "class")) || (token_equals(token, "union"))) {
                        StructType struct_type = StructType_unknown;

                        if(token_equals(token, "struct"))     { struct_type = StructType_struct; }
                        else if(token_equals(token, "class")) { struct_type = StructType_class;  }
                        else if(token_equals(token, "union")) { struct_type = StructType_union;  }

                        if(res.structs.cnt + 1 >= struct_max) {
                            struct_max *= 2;
                            Void *p = system_realloc(res.structs.e, struct_max);
                            if(p) {
                                res.structs.e = cast(StructData *)p;
                            }
                        }

                        ParseStructResult r = parse_struct(&tokenizer, struct_type, template_header);
                        template_header.e = 0; template_header.len = 0;

                        // TODO(Jonny): This fails at a struct declared within a struct/union.
                        if(r.success) { res.structs.e[res.structs.cnt++] = r.sd; }
                    } else if(token_equals(token, "enum")) {
                        if(res.enums.cnt + 1 >= enum_max) {
                            enum_max *= 2;
                            Void *p = system_realloc(res.enums.e, enum_max);
                            if(p) {
                                res.enums.e = cast(EnumData *)p;
                            }
                        }

                        ParseEnumResult r = parse_enum(&tokenizer);
                        if(r.success) {
                            res.enums.e[res.enums.cnt++] = r.ed;
                        }
                    } else if(token_equals(token, "typedef")) {
                        Token original = peak_token(&tokenizer);
                        if(original.type == TokenType_identifier) {
                            eat_token(&tokenizer);

                            Token fresh = peak_token(&tokenizer);
                            if(original.type == TokenType_identifier) {
                                eat_token(&tokenizer);

                                res.typedefs.e[res.typedefs.cnt].original = token_to_string(original);
                                res.typedefs.e[res.typedefs.cnt].fresh = token_to_string(fresh);

                                res.typedefs.cnt++;
                            }
                        }




                    } else {
                        AttemptFunctionResult r = attempt_to_parse_function(token, &tokenizer);
                        if(r.success) {
                            if(res.funcs.cnt + 1 >= func_max) {
                                func_max *= 2;
                                Void *p = system_realloc(res.funcs.e, func_max);
                                if(p) {
                                    res.funcs.e = cast(FunctionData *)p;
                                }
                            }

                            // TODO(Jonny): Realloc if nessessary.
                            res.funcs.e[res.funcs.cnt++] = r.fd;
                        }
                    }
                } break;
            }
        }

        system_free(macro_data);
    }

    return(res);
}
