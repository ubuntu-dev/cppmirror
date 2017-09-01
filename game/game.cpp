#include "pp_generated.h"
#include "sgl_platform.h"

#define SGLM_IMPLEMENTATION
#include "sgl_math.h"

#include <math.h>

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG

PP_IGNORE
#include "stb_image.h"

struct Player {
    sglp_Sprite sprite;
    float x, y;
    float scale_x, scale_y;
    float rot;
};

struct Game_State {
    Player player;
};

enum ID {
    ID_unknown,

    ID_sprite_player,
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

    char buffer[1024] = {};
    pp_serialize_struct(gs, Game_State, buffer, 1024);
    OutputDebugStringA(buffer);
    OutputDebugStringA("\n");

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
    }
}
