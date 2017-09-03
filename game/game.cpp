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

struct Player {
    sglp_Sprite sprite;
    float x, y;
    float scale_x, scale_y;
    float rot;
};

struct Game_State {
    Player player;

    sglp_Sprite text;
};

enum ID {
    ID_unknown,

    ID_sprite_player,
    ID_sprite_text,
    ID_sound_background,
    ID_sound_bloop,
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

void sglp_platform_update_and_render_callback(sglp_API *api) {
    Game_State *gs = (Game_State *)api->permanent_memory;

    {
        int buf_size = 256 * 256;
        char *buffer = new char[buf_size];
        pp_serialize_struct(gs, Game_State, buffer, buf_size);
        OutputDebugStringA(buffer); OutputDebugStringA("\n\n");
        delete buffer;
    }

    if(api->init_game) {
        // Load the player.
        {
            int width, height, number_of_components;
            uint8_t *img_data = stbi_load("player.png", &width, &height, &number_of_components, 0);
            gs->player.sprite = sglp_load_image(api, img_data, 12, ID_sprite_player, width, height, number_of_components);

            gs->player.scale_x = 0.1f;
            gs->player.scale_y = 0.1f;

            gs->player.x = 0.5f;
            gs->player.y = 0.5f;
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

        // Text
        {
            sglp_File ttf_file = api->read_file(api, "c:/Windows/Fonts/LiberationMono-Bold.ttf");
            assert(ttf_file.size);

            stbtt_fontinfo font;
            stbtt_InitFont(&font, ttf_file.e, stbtt_GetFontOffsetForIndex(ttf_file.e, 0));
            int width, height, xoffset, yoffset;
            uint8_t *mono_bitmap = stbtt_GetCodepointBitmap(&font, 0, stbtt_ScaleForPixelHeight(&font, 128.0f),
                                                            'N', &width, &height, &xoffset, &yoffset);


            uint8_t *full_colour_bitmap = (uint8_t *)api->os_malloc(width * height * 4);
            uint8_t *source = mono_bitmap;

            uint8_t *dst_row = full_colour_bitmap;
            for(int32_t i = 0; (i < height); ++i) {
                int32_t *dst = (int32_t *)dst_row;
                for(int J = 0; (J < width); ++J) {
                    uint8_t Alpha = *source++;
                    *dst++ = ((Alpha << 24) | (Alpha << 16) | (Alpha << 8) | (Alpha << 0));
                }

                dst_row += width * 4;
            }

            gs->text = sglp_load_image(api, full_colour_bitmap, 1, ID_sprite_text, width, height, 4);

        }
    }
    else {
        // Update
        if(api->key[sglp_key_space]) {
            sglp_play_audio(api, ID_sound_bloop);
        }


        sglp_clear_screen_for_frame(); // TODO(Jonny): Can this be moved into the platform code?

        if(api->key['W']) gs->player.y -= 0.01f;
        if(api->key['A']) gs->player.x -= 0.01f;
        if(api->key['S']) gs->player.y += 0.01f;
        if(api->key['D']) gs->player.x += 0.01f;

        if(api->key[sglp_key_left])  gs->player.rot += 5.0f;
        if(api->key[sglp_key_right]) gs->player.rot -= 5.0f;

        if(api->key['I']) gs->player.scale_y += 0.01f;
        if(api->key['K']) gs->player.scale_y -= 0.01f;
        if(api->key['J']) gs->player.scale_x -= 0.01f;
        if(api->key['L']) gs->player.scale_x += 0.01f;

        // Render

        sglm_Mat4x4 mat = sglm_mat4x4_set_trans_scale_rot(gs->player.x, gs->player.y,
                                                          gs->player.scale_x, gs->player.scale_y, gs->player.rot);
        float tform[16] = {};
        sglm_mat4x4_as_float_arr(tform, &mat);
        sglp_draw_sprite(gs->player.sprite, 0, tform);


        sglp_draw_sprite(gs->text, 0, tform);
    }
}
