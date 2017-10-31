#include "pp_generated.h"
#include "sgl_platform.h"

#define SGLM_IMPLEMENTATION
#include "sgl_math.h"

#include <math.h>

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

enum Player_Direction : int {
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

#define number_of_enemies 4
struct Game_State {
    sglp_Sprite player_sprite;
    sglp_Sprite enemy_one_sprite;

    Player player;
    Entity enemy[number_of_enemies];

    sglp_Sprite text[26];
};

enum ID {
    ID_unknown,

    ID_sound_bloop,
    ID_sound_background,

    ID_sprite_player,
    ID_sprite_enemy_one,
    ID_sprite_letter,
};

void sglp_platform_setup_settings_callback(sglp_Settings *settings) {
    settings->fullscreen = false;

    settings->win_width  = 640;
    settings->win_height = 480;

    settings->frame_rate = 30;
    settings->permanent_memory_size = sizeof(Game_State);
    settings->max_no_of_sounds = 10;
    settings->window_title = "Hello, Lauren!";
    //settings->thread_cnt;
}

void my_memset(void *dest, uint8_t x, uintptr_t size) {
    int i;
    uint8_t *dest8 = (uint8_t *)dest;
    for(i = 0; (i < size); ++i) {
        dest8[i] = x;
    }
}

void load_all_letters(sglp_API *api, Game_State *gs) {
    sglp_File ttf_file = api->read_file(api, "arial.ttf");
    assert(ttf_file.size);

    stbtt_fontinfo font;
    stbtt_InitFont(&font, ttf_file.e, stbtt_GetFontOffsetForIndex(ttf_file.e, 0));

    uint8_t *full_colour_bitmap = 0;
    uintptr_t size = 0;

    for(int letter_index = 'A'; (letter_index <= 'Z'); ++letter_index) {
        int width, height, xoffset, yoffset;
        uint8_t *mono_bitmap = stbtt_GetCodepointBitmap(&font, 0, stbtt_ScaleForPixelHeight(&font, 128.0f),
                                                        letter_index, &width, &height, &xoffset, &yoffset);

        uintptr_t new_size = width * height * 4;
        if(size < new_size) {
            size = new_size * new_size;
            full_colour_bitmap = (uint8_t *)api->os_realloc(full_colour_bitmap, size);
        }

        uint8_t *source = mono_bitmap;

        uint8_t *dst_row = full_colour_bitmap;
        for(int32_t i = 0; (i < height); ++i) {
            int32_t *dst = (int32_t *)dst_row;
            for(int j = 0; (j < width); ++j) {
                uint8_t alpha = *source++;
                *dst++ = ((alpha << 24) | (alpha << 16) | (alpha << 8) | (alpha << 0));
            }

            dst_row += width * 4;
        }

        gs->text[letter_index - 'A'] = sglp_load_image(api, full_colour_bitmap, 1,
                                                       ID_sprite_letter + (letter_index - 'A'), width, height, 4);
    }

    api->os_free(full_colour_bitmap);
    api->free_file(api, &ttf_file);
}

int32_t string_len(char const *str) {
    int32_t res = 0;
    while(str[res++]);

    return(res);
}

char to_upper(char c) {
    if((c >= 'a') && (c <= 'z')) {
        c -= ('a' - 'A');
    }

    return(c);
}

void draw_word(char const *str, Game_State *gs, float x, float y, float scalex, float scaley) {
    int string_length = string_len(str);
    float running_x = x, running_y = y;
    for(int i = 0; (i < string_length - 1); ++i) {
        char letter = to_upper(str[i]);

        if(letter == '\n') {
            running_y += scaley;
            running_x = x;
        }
        else {
            int letter_index = letter - 'A';

            if((letter >= 'A') || (letter <= 'Z')) {
                sglm_Mat4x4 mat = sglm_mat4x4_set_trans_scale(running_x, running_y, scalex, scaley);

                float tform[16] = {};
                sglm_mat4x4_as_float_arr(tform, &mat);

                sglp_draw_sprite(gs->text[letter_index], 0, tform);

                running_x += scalex;
            }
        }
    }
}

bool overlap(Entity a, Entity b) {
    if((a.x >= b.x && a.x <= b.x + b.scale_x) || (b.x >= a.x && b.x <= a.x + a.scale_x)) {
        if((a.y >= b.y && a.y <= b.y + b.scale_y) || (b.y >= a.y && b.y <= a.y + a.scale_y)) {
            return(true);
        }
    }

    return(false);
}

void sglp_platform_update_and_render_callback(sglp_API *api) {
    Game_State *gs = (Game_State *)api->permanent_memory;

    {
        int buf_size = 256 * 256;
        char *buffer = new char[buf_size];
        pp_serialize_struct(gs, Game_State, buffer, buf_size);
        //OutputDebugStringA(buffer); OutputDebugStringA("\n\n");
        delete[] buffer;
    }

    if(api->init_game) {
        // Load the player.
        {
            int width, height, number_of_components;
            uint8_t *img_data = stbi_load("player.png", &width, &height, &number_of_components, 0);
            gs->player_sprite = sglp_load_image(api, img_data, 12, ID_sprite_player, width, height, number_of_components);
            stbi_image_free(img_data);

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
            gs->enemy_one_sprite = sglp_load_image(api, img_data, 8, ID_sprite_enemy_one, width, height, number_of_components);
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

        // Load background music.
        {
            sglp_File background_wav = api->read_file(api, "music_test.wav");
            bool success = sglp_load_wav(api, ID_sound_background, background_wav.e, background_wav.size);
            if(success) {
                api->free_file(api, &background_wav);
                sglp_play_audio(api, ID_sound_background);
            }
        }

        // Load bloop.
        {
            sglp_File bloop_wav = api->read_file(api, "bloop_00.wav");
            bool success = sglp_load_wav(api, ID_sound_bloop, bloop_wav.e, bloop_wav.size);
            if(success) {
                api->free_file(api, &bloop_wav);
            }
        }

        load_all_letters(api, gs);
    }
    else {
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

        if(api->key[sglp_key_left])  gs->player.trans.rot += 5.0f;
        if(api->key[sglp_key_right]) gs->player.trans.rot -= 5.0f;

        if(api->key['I']) gs->player.trans.scale_y += 0.01f;
        if(api->key['K']) gs->player.trans.scale_y -= 0.01f;
        if(api->key['J']) gs->player.trans.scale_x -= 0.01f;
        if(api->key['L']) gs->player.trans.scale_x += 0.01f;

        for(int i = 0; (i < number_of_enemies); ++i) {
            if(overlap(gs->player.trans, gs->enemy[i])) {
                gs->player.trans.x = gs->player.start_x;
                gs->player.trans.y = gs->player.start_y;
            }
        }

        gs->player.current_frame += 0.5f;
        if(gs->player.current_frame >= gs->player.dir + 2.0f) {
            gs->player.current_frame = gs->player.dir;
        }

        // Render
        {
            sglm_Mat4x4 mat = sglm_mat4x4_set_trans_scale_rot(gs->player.trans.x, gs->player.trans.y,
                                                              gs->player.trans.scale_x, gs->player.trans.scale_y,
                                                              gs->player.trans.rot);
            float tform[16] = {};
            sglm_mat4x4_as_float_arr(tform, &mat);

            sglp_draw_sprite(gs->player_sprite, gs->player.current_frame, tform);
        }

        for(int i = 0; (i < number_of_enemies); ++i) {
            sglm_Mat4x4 mat = sglm_mat4x4_set_trans_scale_rot(gs->enemy[i].x, gs->enemy[i].y,
                                                              gs->enemy[i].scale_x, gs->enemy[i].scale_y, gs->enemy[i].rot);
            float tform[16] = {};
            sglm_mat4x4_as_float_arr(tform, &mat);

            sglp_draw_sprite(gs->enemy_one_sprite, 0, tform);
        }

        draw_word("abcdefghi\njklmnopq\nrstuvwxyz", gs, 0.1f, 0.1f, 0.1f, 0.1f);
    }
}
