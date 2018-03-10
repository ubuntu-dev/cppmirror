#define INTERNAL 1

#include "pp_generated.h"

#define SGLP_IMPLEMENTATION
//#include <SDL2/SDL.h>
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

struct V2 {
    float x, y;
};
V2 v2(Float x, Float y) {
    V2 res = { .x = x, .y = y };
    
    return(res);
}

struct Transform {
    V2 pos;
    V2 scale;
    Float rot;
};

enum Direction {
    Direction_unknown,
    Direction_left,
    Direction_right,
    Direction_up,
    Direction_down,
};

struct Bullet {
    Transform trans;
    Direction dir;
};
Bullet bullet(void) {
    Bullet res = {0};
    res.trans.scale.x = 0.025f;
    res.trans.scale.y = 0.025f;

    return(res);
}

// The player direction is a little complicated because it's used for the animation.
enum Player_Direction {
    Player_Direction_left = 0,
    Player_Direction_right = 2,
    Player_Direction_up = 4,
    Player_Direction_down = 6,
};
struct Player {
    Transform trans;
    V2 start_pos;
    Player_Direction dir;
    Float current_frame;
    V2 current_speed;

    Bullet bullet;
    Bool is_shooting;
};

Direction player_direction_to_direction(Player_Direction pd) {
    Direction d = 0;
    if(0) {}
    else if(pd == Player_Direction_left)  { d = Direction_left;  }
    else if(pd == Player_Direction_right) { d = Direction_right; }
    else if(pd == Player_Direction_up)    { d = Direction_up;    }
    else if(pd == Player_Direction_down)  { d = Direction_down;  }

    return(d);
}

struct Enemy {
    Transform trans;
    Bool valid;
};

#define NUMBER_OF_ENEMIES 4
struct Game_State {
    sglp_Sprite player_sprite;
    sglp_Sprite enemy_one_sprite;
    sglp_Sprite bitmap_sprite;
    sglp_Sprite bullet_sprite;

    Player player;
    Enemy enemy[NUMBER_OF_ENEMIES];
};

enum ID {
    ID_unknown,

    ID_sound_bloop,
    ID_sound_background,

    ID_sprite_player,
    ID_sprite_enemy_one,
    ID_sprite_bitmap_font,
    ID_sprite_bullet,
};

void sglp_platform_setup_settings_callback(sglp_Settings *settings) {
    settings->fullscreen = false;

    //settings->win_width  = 640;
    //settings->win_height = 480;

    settings->frame_rate = 30;
    settings->permanent_memory_size = sizeof(Game_State);
    // TODO - settings->disable_sound = true;
    settings->max_no_of_sounds = 10;
    settings->window_title = "Game stuff";
    settings->thread_cnt = 8;
}

Float accelerate(Float cur, Float max, Float acc, Bool forward) {
    Float go_forward = (forward) ? 1.0f : -1.0f;
    Float res = ((go_forward * max) - cur) * acc;
    if(res > max) {
        res = max;
    }

    return(res);
}

V2 get_letter_position(char Letter);
void draw_word(char const *str, sglp_API *api, Game_State *gs, Float x, Float y, Float scalex, Float scaley) {
    scalex *= 0.5f;
    int string_length = sgl_string_len(str);
    Float running_x = x, running_y = y;
    for(int i = 0; (i < string_length - 1); ++i) {
        char letter = str[i];

        if(letter == '\n') {
            running_y += scaley;
            running_x = x;
        } else {
            V2 pos_in_table = get_letter_position(letter);
            if(pos_in_table.x != -1 && pos_in_table.y != -1) {
                sglm_Mat4x4 mat = sglm_mat4x4_set_trans_scale(running_x, running_y, scalex, scaley);
                float tform[16] = {0};
                
                sglm_mat4x4_as_float_arr(tform, &mat);
                sglp_draw_sprite_frame_matrix(gs->bitmap_sprite, pos_in_table.x, pos_in_table.y, tform);

                running_x += scalex;
            }
        }
    }
}

Bool overlap(Transform a, Transform b) {
    if((a.pos.x >= b.pos.x && a.pos.x <= b.pos.x + b.scale.x) || (b.pos.x >= a.pos.x && b.pos.x <= a.pos.x + a.scale.x)) {
        if((a.pos.y >= b.pos.y && a.pos.y <= b.pos.y + b.scale.y) || (b.pos.y >= a.pos.y && b.pos.y <= a.pos.y + a.scale.y)) {
            return(true);
        }
    }

    return(false);
}

void draw_debug_information(sglp_API *api, Game_State *gs, Float mouse_x, Float mouse_y) {
#if INTERNAL
    int buf_size = 256 * 256;
    char *buffer = api->os_malloc(sizeof(*buffer) * buf_size);
    pp_serialize_struct(&gs->player, Player, buffer, buf_size);
    Float size = 0.025f;
    draw_word(buffer, api, gs, 0.0f, 0.0f, size, size);
    api->os_free(buffer);
#endif
}

void render(sglp_API *api, Game_State *gs) {
    sglp_clear_screen_for_frame();
    
    // Player
    {
        sglm_Mat4x4 mat = sglm_mat4x4_set_trans_scale_rot(gs->player.trans.pos.x, gs->player.trans.pos.y,
                                                          gs->player.trans.scale.x, gs->player.trans.scale.y,
                                                          gs->player.trans.rot);
        Float tform[16] = {0};
        sglm_mat4x4_as_float_arr(tform, &mat);

        sglp_draw_sprite(gs->player_sprite, gs->player.current_frame, tform);
    }
    
    // Player's Bullet
    if(gs->player.is_shooting) {
        Bullet *b = &gs->player.bullet;
        sglm_Mat4x4 mat = sglm_mat4x4_set_trans_scale_rot(b->trans.pos.x, b->trans.pos.y,
                                                          b->trans.scale.x, b->trans.scale.y,
                                                          b->trans.rot);
        Float tform[16] = {0};
        sglm_mat4x4_as_float_arr(tform, &mat);

        sglp_draw_sprite(gs->bullet_sprite, 0, tform);
    }
    
    // Enemies
    for(int i = 0; (i < NUMBER_OF_ENEMIES); ++i) {
        sglm_Mat4x4 mat = sglm_mat4x4_set_trans_scale_rot(gs->enemy[i].trans.pos.x, gs->enemy[i].trans.pos.y,
                                                          gs->enemy[i].trans.scale.x, gs->enemy[i].trans.scale.y,
                                                          gs->enemy[i].trans.rot);
        Float tform[16] = {0};
        sglm_mat4x4_as_float_arr(tform, &mat);

        sglp_draw_sprite(gs->enemy_one_sprite, 0, tform);
    }
    
    // TODO - Read the mouse position from sgl_platform.
    draw_debug_information(api, gs, 0.0f, 0.0f);
}

Enemy create_enemy(float x, float y) {
    Enemy res = {0};

    res.trans.scale.x = 0.1f;
    res.trans.scale.y = 0.1f;

    res.trans.pos.x = x;
    res.trans.pos.y = y;

    return(res);
}

Player create_player(float x, float y) {
    Player res = {0};

    res.trans.scale.x = 0.1f;
    res.trans.scale.y = 0.1f;

    res.trans.pos.x = x;
    res.trans.pos.y = y;

    res.start_pos = res.trans.pos;
    res.dir = Player_Direction_left;

    return(res);
}

void init(sglp_API *api, Game_State *gs) {
    // Load the player.
    {
        int width, height, number_of_components;
        uint8_t *img_data = stbi_load("player.png", &width, &height, &number_of_components, 0);
        gs->player_sprite = sglp_load_image(api, img_data, 12, 1, ID_sprite_player, width, height, number_of_components);
        stbi_image_free(img_data);

        gs->player = create_player(0.5f, 0.7f);
    }

    // Load the enemy.
    {
        int width, height, number_of_components;
        uint8_t *img_data = stbi_load("enemy_one.png", &width, &height, &number_of_components, 0);
        gs->enemy_one_sprite = sglp_load_image(api, img_data, 8, 1, ID_sprite_enemy_one, width, height, number_of_components);
        stbi_image_free(img_data);

        Float x = 0.1f;
        for(Int i = 0; (i < 4); ++i) {
            gs->enemy[i] = create_enemy(x, 0.5f);
            x += 0.2f;
        }
    }

    // Load font.
    {
        int width, height, number_of_components;
        uint8_t *img_data = stbi_load("freemono.png", &width, &height, &number_of_components, 0);
        gs->bitmap_sprite = sglp_load_image(api, img_data, 16, 16, ID_sprite_bitmap_font, width, height, number_of_components);
        stbi_image_free(img_data);
    }

    // Load bullet
    {
        Int width, height, number_of_components;
        uint8_t *img_data = stbi_load("bullet.png", &width, &height, &number_of_components, 0);
        gs->bullet_sprite = sglp_load_image(api, img_data, 1, 1, ID_sprite_bullet, width, height, number_of_components);
        stbi_image_free(img_data);

        gs->player.bullet = bullet();
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
}

void update(sglp_API *api, Game_State *gs) {
    // Update
    if(api->key[sglp_key_space]) {
        sglp_play_audio(api, ID_sound_bloop);
    }

    V2 max_speed = v2(0.01f, 0.01f);
    V2 acceleration = v2(0.006f, 0.006f);
    V2 friction = v2(0.005f, 0.005f);

    V2 current_speed = gs->player.current_speed;
    if(api->key['W']) {
        current_speed.y += accelerate(current_speed.y, max_speed.y, acceleration.y * api->dt, false);
        gs->player.dir = Player_Direction_up;
        gs->player.current_frame = Player_Direction_up;
    } else if(api->key['S']) {
        current_speed.y += accelerate(current_speed.y, max_speed.y, acceleration.y * api->dt, true);
        gs->player.dir = Player_Direction_down;
        gs->player.current_frame = Player_Direction_down;
    } else {
        if(current_speed.y > friction.y * 0.5f) {
            current_speed.y += accelerate(current_speed.y, max_speed.y, friction.y * api->dt, false);
        } else if(current_speed.y < -friction.y * 0.5f) {
            current_speed.y += accelerate(current_speed.y, max_speed.y, friction.y * api->dt, true);
        } else {
            current_speed.y = 0;
        }
    }

    if(api->key['A']) {
        current_speed.x += accelerate(current_speed.x, max_speed.x, acceleration.x * api->dt, false);
        gs->player.dir = Player_Direction_left;
        gs->player.current_frame = Player_Direction_left;
    } else if(api->key['D']) {
        current_speed.x += accelerate(current_speed.x, max_speed.x, acceleration.x * api->dt, true);
        gs->player.dir = Player_Direction_right;
        gs->player.current_frame = Player_Direction_right;
    } else {
        if(current_speed.x > friction.x * 0.5f) {
            current_speed.x += accelerate(current_speed.x, max_speed.x, friction.x * api->dt, false);
        } else if(current_speed.x < -friction.x * 0.5f) {
            current_speed.x += accelerate(current_speed.x, max_speed.x, friction.x * api->dt, true);
        } else {
            current_speed.x = 0;
        }
    }

    gs->player.current_speed = current_speed;
    gs->player.trans.pos.x += gs->player.current_speed.x;
    gs->player.trans.pos.y += gs->player.current_speed.y;

    if(api->key[sglp_key_space]) {
        if(!gs->player.is_shooting) {
            gs->player.is_shooting = true;
            gs->player.bullet.dir = player_direction_to_direction(gs->player.dir);
            gs->player.bullet.trans = gs->player.trans;
        }
    }

    if(gs->player.is_shooting) {
        Float bullet_speed = 0.02f;
        if(gs->player.bullet.dir == Direction_left)  { gs->player.bullet.trans.pos.x -= bullet_speed; }
        if(gs->player.bullet.dir == Direction_right) { gs->player.bullet.trans.pos.x += bullet_speed; }
        if(gs->player.bullet.dir == Direction_up)    { gs->player.bullet.trans.pos.y -= bullet_speed; }
        if(gs->player.bullet.dir == Direction_down)  { gs->player.bullet.trans.pos.y += bullet_speed; }
    }

    float rot_speed = 5.0f;
    if(api->key[sglp_key_left])  { gs->player.trans.rot += rot_speed; }
    if(api->key[sglp_key_right]) { gs->player.trans.rot -= rot_speed; }

    float scaling_speed = 0.01f;
    if(api->key['I']) { gs->player.trans.scale.y += scaling_speed; }
    if(api->key['K']) { gs->player.trans.scale.y -= scaling_speed; }
    if(api->key['J']) { gs->player.trans.scale.x -= scaling_speed; }
    if(api->key['L']) { gs->player.trans.scale.x += scaling_speed; }

    for(int i = 0; (i < NUMBER_OF_ENEMIES); ++i) {
        if(overlap(gs->player.trans, gs->enemy[i].trans)) {
            gs->player.trans.pos = gs->player.start_pos;
        }
    }

    gs->player.current_frame += 0.5f;
    if(gs->player.current_frame >= gs->player.dir + 2.0f) {
        gs->player.current_frame = gs->player.dir;
    }
}

void sglp_platform_update_and_render_callback(sglp_API *api) {
    Game_State *gs = (Game_State *)api->permanent_memory;

    if(api->init_game) {
        init(api, gs);
    } else {
        update(api, gs);
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

