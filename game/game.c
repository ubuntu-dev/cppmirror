#define INTERNAL 1

#include "pp_generated.h"

#define SGLP_IMPLEMENTATION
#include "sgl_platform.h"

#define SGLM_IMPLEMENTATION
#include "sgl_math.h"

#define SGL_IMPLEMENTATION
#include "sgl.h"

#include <math.h>

#define PP_IGNORE
PP_IGNORE
#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#include "stb_image.h"

PP_IGNORE
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_ttf.h"

struct Entity {
    float x, y;
    float scale_x, scale_y;
    float rot;
};

enum Player_Direction {
    Player_Direction_left = 0,
    Player_Direction_right = 2,
    Player_Direction_up = 4,
    Player_Direction_down = 6,
};

struct Player {
    Entity trans;
    float start_x, start_y;
    Player_Direction dir;
    float current_frame;
};

#define NUMBER_OF_ENEMIES 4
struct Game_State {
    sglp_Sprite player_sprite;
    sglp_Sprite enemy_one_sprite;
    sglp_Sprite bitmap_sprite;

    Player player;
    Entity enemy[NUMBER_OF_ENEMIES];
};

enum ID {
    ID_unknown,

    ID_sound_bloop,
    ID_sound_background,

    ID_sprite_player,
    ID_sprite_enemy_one,
    ID_sprite_bitmap_font,
};

void sglp_platform_setup_settings_callback(sglp_Settings *settings) {
    settings->fullscreen = true;

    //settings->win_width  = 640;
    //settings->win_height = 480;

    settings->frame_rate = 30;
    settings->permanent_memory_size = sizeof(Game_State);
    settings->max_no_of_sounds = 10;
    settings->window_title = "Hello, Lauren!";
    //settings->thread_cnt;
}
struct V2 {
    float x, y;
};

V2 v2(float x, float y) {
    V2 res = { .x = x, .y = y };
    return(res);
}

V2 get_letter_position(char Letter);

void draw_word(char const *str, sglp_API *api, Game_State *gs, float x, float y, float scalex, float scaley) {
    scalex *= 0.5f;
    int string_length = sgl_string_len(str);
    float running_x = x, running_y = y;
    for(int i = 0; (i < string_length - 1); ++i) {
        char letter = str[i];

        if(letter == '\n') {
            running_y += scaley;
            running_x = x;
        } else {
            V2 pos_in_table = get_letter_position(letter);
            if(pos_in_table.x != -1 && pos_in_table.y != -1) {
                sglm_Mat4x4 mat = sglm_mat4x4_set_trans_scale(running_x, running_y, scalex, scaley);
                float tform[16] = {};
                sglm_mat4x4_as_float_arr(tform, &mat);
                sglp_draw_sprite_frame_matrix(gs->bitmap_sprite, pos_in_table.x, pos_in_table.y, tform);

                running_x += scalex;
            }
        }
    }
}

Bool overlap(Entity a, Entity b) {
    if((a.x >= b.x && a.x <= b.x + b.scale_x) || (b.x >= a.x && b.x <= a.x + a.scale_x)) {
        if((a.y >= b.y && a.y <= b.y + b.scale_y) || (b.y >= a.y && b.y <= a.y + a.scale_y)) {
            return(true);
        }
    }

    return(false);
}

void draw_debug_information(sglp_API *api, Game_State *gs, float mouse_x, float mouse_y) {
#if INTERNAL
    int buf_size = 256 * 256;
    char *buffer = api->os_malloc(sizeof *buffer * buf_size);
    pp_serialize_struct(&gs->player, Player, buffer, buf_size);
    float size = 0.05f;
    draw_word(buffer, api, gs, 0.0f, 0.0f, size, size);
    api->os_free(buffer);
#endif
}

void render(sglp_API *api, Game_State *gs) {
    // Player
    {
        sglm_Mat4x4 mat = sglm_mat4x4_set_trans_scale_rot(gs->player.trans.x, gs->player.trans.y,
                                                          gs->player.trans.scale_x, gs->player.trans.scale_y,
                                                          gs->player.trans.rot);
        float tform[16] = {};
        sglm_mat4x4_as_float_arr(tform, &mat);

        sglp_draw_sprite(gs->player_sprite, gs->player.current_frame, tform);
    }

    // Enemies
    for(int i = 0; (i < NUMBER_OF_ENEMIES); ++i) {
        sglm_Mat4x4 mat = sglm_mat4x4_set_trans_scale_rot(gs->enemy[i].x, gs->enemy[i].y,
                                                          gs->enemy[i].scale_x, gs->enemy[i].scale_y, gs->enemy[i].rot);
        float tform[16] = {};
        sglm_mat4x4_as_float_arr(tform, &mat);

        sglp_draw_sprite(gs->enemy_one_sprite, 0, tform);
    }

    // TODO - Read the mouse position from sgl_platform.
    draw_debug_information(api, gs, 0.0f, 0.0f);
}

void sglp_platform_update_and_render_callback(sglp_API *api) {
    Game_State *gs = (Game_State *)api->permanent_memory;

    if(api->init_game) {
        // Load the player.
        {
            int width, height, number_of_components;
            uint8_t *img_data = stbi_load("player.png", &width, &height, &number_of_components, 0);
            gs->player_sprite = sglp_load_image(api, img_data, 12, 1, ID_sprite_player, width, height, number_of_components);
            stbi_image_free(img_data);

            int c;

            gs->player.trans.scale_x = 0.1f;
            gs->player.trans.scale_y = 0.1f;

            gs->player.trans.x = 0.5f;
            gs->player.trans.y = 0.7f;

            gs->player.start_x = gs->player.trans.x;
            gs->player.start_y = gs->player.trans.y;
        }

        // Load the enemy.
        {
            int width, height, number_of_components;
            uint8_t *img_data = stbi_load("enemy_one.png", &width, &height, &number_of_components, 0);
            gs->enemy_one_sprite = sglp_load_image(api, img_data, 8, 1, ID_sprite_enemy_one, width, height, number_of_components);
            stbi_image_free(img_data);

            float x = 0.1f;
            for(int i = 0; (i < 4); ++i) {
                gs->enemy[i].scale_x = 0.1f;
                gs->enemy[i].scale_y = 0.1f;

                gs->enemy[i].x = x;
                gs->enemy[i].y = 0.5f;

                x += 0.4f;
            }
        }

        {
            int width, height, number_of_components;
            uint8_t *img_data = stbi_load("freemono.png", &width, &height, &number_of_components, 0);
            gs->bitmap_sprite = sglp_load_image(api, img_data, 16, 16, ID_sprite_bitmap_font, width, height, number_of_components);
            stbi_image_free(img_data);
        }

        // Load background music.
        {
            sglp_File background_wav = api->read_file(api, "music_test.wav");
            Bool success = sglp_load_wav(api, ID_sound_background, background_wav.e, background_wav.size);
            if(success) {
                api->free_file(api, &background_wav);
                sglp_play_audio(api, ID_sound_background);
            }
        }

        // Load bloop.
        {
            sglp_File bloop_wav = api->read_file(api, "bloop_00.wav");
            Bool success = sglp_load_wav(api, ID_sound_bloop, bloop_wav.e, bloop_wav.size);
            if(success) {
                api->free_file(api, &bloop_wav);
            }
        }

        //load_all_letters(api, gs);
    } else {
        // Update
        if(api->key[sglp_key_space]) {
            sglp_play_audio(api, ID_sound_bloop);
        }

        sglp_clear_screen_for_frame(); // TODO(Jonny): Can this be moved into the platform code?

        if(api->key['W']) {
            gs->player.trans.y -= 0.01f;
            gs->player.dir = Player_Direction_up;
            gs->player.current_frame = Player_Direction_up;
        }
        if(api->key['A']) {
            gs->player.trans.x -= 0.01f;
            gs->player.dir = Player_Direction_left;
            gs->player.current_frame = Player_Direction_left;
        }
        if(api->key['S']) {
            gs->player.trans.y += 0.01f;
            gs->player.dir = Player_Direction_down;
            gs->player.current_frame = Player_Direction_down;
        }
        if(api->key['D']) {
            gs->player.trans.x += 0.01f;
            gs->player.dir = Player_Direction_right;
            gs->player.current_frame = Player_Direction_right;
        }

        if(api->key[sglp_key_left]) { gs->player.trans.rot += 5.0f; }
        if(api->key[sglp_key_right]) { gs->player.trans.rot -= 5.0f; }

        if(api->key['I']) { gs->player.trans.scale_y += 0.01f; }
        if(api->key['K']) { gs->player.trans.scale_y -= 0.01f; }
        if(api->key['J']) { gs->player.trans.scale_x -= 0.01f; }
        if(api->key['L']) { gs->player.trans.scale_x += 0.01f; }

        for(int i = 0; (i < NUMBER_OF_ENEMIES); ++i) {
            if(overlap(gs->player.trans, gs->enemy[i])) {
                gs->player.trans.x = gs->player.start_x;
                gs->player.trans.y = gs->player.start_y;
            }
        }

        gs->player.current_frame += 0.5f;
        if(gs->player.current_frame >= gs->player.dir + 2.0f) {
            gs->player.current_frame = gs->player.dir;
        }

        render(api, gs);
    }
}

V2 get_letter_position(char letter) {
    V2 res = v2(-1.0f, -1.0f);
    switch (letter) {
        case ' ': { res = v2(0.0f, 0.0f);  } break;
        case '!': { res = v2(1.0f, 0.0f);  } break;
        case '"': { res = v2(2.0f, 0.0f);  } break;
        case '#': { res = v2(3.0f, 0.0f);  } break;
        case '$': { res = v2(4.0f, 0.0f);  } break;
        case '%': { res = v2(5.0f, 0.0f);  } break;
        case '&': { res = v2(6.0f, 0.0f);  } break;
        case '(': { res = v2(8.0f, 0.0f);  } break;
        case ')': { res = v2(9.0f, 0.0f);  } break;
        case '*': { res = v2(10.0f, 0.0f); } break;
        case '+': { res = v2(11.0f, 0.0f); } break;
        case '_': { res = v2(13.0f, 0.0f); } break;
        case '/': { res = v2(15.0f, 0.0f); } break;
        case '0': { res = v2(0.0f, 1.0f);  } break;
        case '1': { res = v2(1.0f, 1.0f);  } break;
        case '2': { res = v2(2.0f, 1.0f);  } break;
        case '3': { res = v2(3.0f, 1.0f);  } break;
        case '4': { res = v2(4.0f, 1.0f);  } break;
        case '5': { res = v2(5.0f, 1.0f);  } break;
        case '6': { res = v2(6.0f, 1.0f);  } break;
        case '7': { res = v2(7.0f, 1.0f);  } break;
        case '8': { res = v2(8.0f, 1.0f);  } break;
        case '9': { res = v2(9.0f, 1.0f);  } break;
        case ':': { res = v2(10.0f, 1.0f); } break;
        case '.': { res = v2(10.0f, 1.0f); } break; // TODO(Jonny): Hacky
        case ',': { res = v2(10.0f, 1.0f); } break; // TODO(Jonny): Hacky
        case ';': { res = v2(11.0f, 1.0f); } break;
        case '<': { res = v2(12.0f, 1.0f); } break;
        case '=': { res = v2(13.0f, 1.0f); } break;
        case '>': { res = v2(14.0f, 1.0f); } break;
        case '?': { res = v2(15.0f, 1.0f); } break;
        case '@': { res = v2(0.0f, 2.0f);  } break;
        case 'A': { res = v2(1.0f, 2.0f);  } break;
        case 'B': { res = v2(2.0f, 2.0f);  } break;
        case 'C': { res = v2(3.0f, 2.0f);  } break;
        case 'D': { res = v2(4.0f, 2.0f);  } break;
        case 'E': { res = v2(5.0f, 2.0f);  } break;
        case 'F': { res = v2(6.0f, 2.0f);  } break;
        case 'G': { res = v2(7.0f, 2.0f);  } break;
        case 'H': { res = v2(8.0f, 2.0f);  } break;
        case 'I': { res = v2(9.0f, 2.0f);  } break;
        case 'J': { res = v2(10.0f, 2.0f); } break;
        case 'K': { res = v2(11.0f, 2.0f); } break;
        case 'L': { res = v2(12.0f, 2.0f); } break;
        case 'M': { res = v2(13.0f, 2.0f); } break;
        case 'N': { res = v2(14.0f, 2.0f); } break;
        case 'O': { res = v2(15.0f, 2.0f); } break;
        case 'P': { res = v2(0.0f, 3.0f);  } break;
        case 'Q': { res = v2(1.0f, 3.0f);  } break;
        case 'R': { res = v2(2.0f, 3.0f);  } break;
        case 'S': { res = v2(3.0f, 3.0f);  } break;
        case 'T': { res = v2(4.0f, 3.0f);  } break;
        case 'U': { res = v2(5.0f, 3.0f);  } break;
        case 'V': { res = v2(6.0f, 3.0f);  } break;
        case 'W': { res = v2(7.0f, 3.0f);  } break;
        case 'X': { res = v2(8.0f, 3.0f);  } break;
        case 'Y': { res = v2(9.0f, 3.0f);  } break;
        case 'Z': { res = v2(10.0f, 3.0f); } break;
        case '[': { res = v2(11.0f, 3.0f); } break;
        case ']': { res = v2(13.0f, 3.0f); } break;
        case '^': { res = v2(14.0f, 3.0f); } break;
        case '-': { res = v2(15.0f, 3.0f); } break;
        case 'a': { res = v2(1.0f, 4.0f);  } break;
        case 'b': { res = v2(2.0f, 4.0f);  } break;
        case 'c': { res = v2(3.0f, 4.0f);  } break;
        case 'd': { res = v2(4.0f, 4.0f);  } break;
        case 'e': { res = v2(5.0f, 4.0f);  } break;
        case 'f': { res = v2(6.0f, 4.0f);  } break;
        case 'g': { res = v2(7.0f, 4.0f);  } break;
        case 'h': { res = v2(8.0f, 4.0f);  } break;
        case 'i': { res = v2(9.0f, 4.0f);  } break;
        case 'j': { res = v2(10.0f, 4.0f); } break;
        case 'k': { res = v2(11.0f, 4.0f); } break;
        case 'l': { res = v2(12.0f, 4.0f); } break;
        case 'm': { res = v2(13.0f, 4.0f); } break;
        case 'n': { res = v2(14.0f, 4.0f); } break;
        case 'o': { res = v2(15.0f, 4.0f); } break;
        case 'p': { res = v2(0.0f, 5.0f);  } break;
        case 'q': { res = v2(1.0f, 5.0f);  } break;
        case 'r': { res = v2(2.0f, 5.0f);  } break;
        case 's': { res = v2(3.0f, 5.0f);  } break;
        case 't': { res = v2(4.0f, 5.0f);  } break;
        case 'u': { res = v2(5.0f, 5.0f);  } break;
        case 'v': { res = v2(6.0f, 5.0f);  } break;
        case 'w': { res = v2(7.0f, 5.0f);  } break;
        case 'x': { res = v2(8.0f, 5.0f);  } break;
        case 'y': { res = v2(9.0f, 5.0f);  } break;
        case 'z': { res = v2(10.0f, 5.0f); } break;
        case '{': { res = v2(11.0f, 5.0f); } break;
        case '|': { res = v2(12.0f, 5.0f); } break;
        case '}': { res = v2(13.0f, 5.0f); } break;
        case '~': { res = v2(14.0f, 5.0f); } break;

        default: { assert(false); } break;
    }

    return(res);
}

