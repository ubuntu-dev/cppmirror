#define STB_SPRINTF_IMPLEMENTATION
#include "stb_sprintf.h"

#define PP_ASSERT(x) // TODO - Implement.
#define PP_SPRINTF stbsp_snprintf
#include "pp_generated.h"

// #define SGLP_IMPLEMENTATION
#include "sgl_platform.h"

#define SGLM_IMPLEMENTATION
#include "sgl_math.h"

#define SGL_IMPLEMENTATION
#define SGL_NO_CRT_DLL
#include "sgl.h"

static sglp_API *global_api;

#define STBI_NO_STDIO
#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#define STBI_ASSERT SGL_ASSERT
#define STBI_MALLOC global_api->os_malloc
#define STBI_REALLOC global_api->os_realloc
#define STBI_FREE global_api->os_free
PP_IGNORE
#include "stb_image.h"

#define assert(x) SGL_ASSERT(x)

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
    Void *parent;
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

    Bool can_shoot;
    Int shot_timer;
};

Direction player_direction_to_direction(Player_Direction pd) {
    Direction d = 0;
    if(0) {}
    else if(pd == Player_Direction_left)  d = Direction_left;
    else if(pd == Player_Direction_right) d = Direction_right;
    else if(pd == Player_Direction_up)    d = Direction_up;
    else if(pd == Player_Direction_down)  d = Direction_down;

    return(d);
}

struct Enemy {
    Transform trans;
};

struct Entity {
    // These have to be first in the struct, so I can cast between them.
    union {
        Player player;
        Enemy enemy;
        Bullet bullet;
    };

    pp_Type type;
    Entity *next;
};


Entity *get_end_entity(Entity *root) {
    Entity *next = root;
    if(!root) {
        next = root;
    }
    else {
        while(next->next) {
            next = next->next;
        }
    }

    return next;
}

Entity *push_entity(sglp_API *api, Entity *root, Void *var, pp_Type type) {
    Entity *next = api->sglp_push_permanent_struct(api, Entity);
    uintptr_t size = pp_get_size_from_type(type);

    Entity *end = get_end_entity(root);
    if(!end) {
        root = next;
    }
    else {
        end->next = next;
    }

    sgl_memcpy(next, var, size);
    next->type = type;

    return root;
}

Void *find_first_entity(Entity *root, pp_Type type) {
    Entity *next = root;

    while(next) {
        if(next->type == type) {
            Uintptr number_of_members = pp_get_number_of_members(pp_Type_Entity);
            for(Uintptr j = 0; (j < number_of_members); ++j) {
                pp_MemberDefinition mem = pp_get_members_from_type(pp_Type_Entity, j);
                if(mem.type == type) {
                    Void *res = (Byte *)next + mem.offset;
                    return res;
                }
            }
        }

        next = next->next;
    }

    return 0;
}

// This can take the root to find the first entity.
Void *find_next_entity(Void *root, pp_Type type) {
    if(root) {
        Entity *next = root;
        return find_first_entity(next->next, type);
    }

    return 0;
}


enum Sprite_ID {
    Sprite_ID_unknown,

    Sprite_ID_player,
    Sprite_ID_enemy_one,
    Sprite_ID_bitmap_font,
    Sprite_ID_bullet,
};

struct Game_State {
    sglp_Sprite sprite[pp_get_enum_size_const(Sprite_ID)];

    Entity *entity;
};

enum Sound_ID {
    ID_unknown,

    ID_sound_bloop,
    ID_sound_background,
};

void sglp_platform_setup_settings_callback(sglp_Settings *settings) {
    settings->fullscreen = false;

    settings->allow_sound = false;

    //settings->win_width  = 640;
    //settings->win_height = 480;

    settings->frame_rate = 60;
    settings->game_state_memory_size = sizeof(Game_State);
    settings->permanent_memory_size = SGL_MEGABYTES(1);
    settings->temp_memory_size = SGL_MEGABYTES(1);
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
void draw_word(char const *str, sglp_API *api, Game_State *gs, V2 pos, V2 scale) {
    scale.x *= 0.5f;
    Int string_length = sgl_string_len(str);
    Float running_x = pos.x;
    Float running_y = pos.y;
    for(Int i = 0; (i < string_length - 1); ++i) {
        Char letter = str[i];

        if(letter == '\n') {
            running_y += scale.y;
            running_x = pos.x;
        }
        else {
            V2 pos_in_table = get_letter_position(letter);
            if(pos_in_table.x != -1 && pos_in_table.y != -1) {
                sglm_Mat4x4 mat = sglm_mat4x4_set_trans_scale(running_x, running_y, scale.x, scale.y);
                float tform[16] = {0};

                sglm_mat4x4_as_float_arr(tform, &mat);
                api->draw_sprite_frame_matrix(gs->sprite[Sprite_ID_bitmap_font], pos_in_table.x, pos_in_table.y, tform);

                running_x += scale.x;
            }
        }
    }
}

Bool overlap(Transform a, Transform b) {
    if((a.pos.x >= b.pos.x && a.pos.x <= b.pos.x + b.scale.x) || (b.pos.x >= a.pos.x && b.pos.x <= a.pos.x + a.scale.x)) {
        if((a.pos.y >= b.pos.y && a.pos.y <= b.pos.y + b.scale.y) || (b.pos.y >= a.pos.y && b.pos.y <= a.pos.y + a.scale.y)) {
            return true;
        }
    }

    return false;
}

Bool point_overlap(V2 p, Transform t) {
    Float x = t.pos.x - (t.scale.x * 0.5f);
    Float y = t.pos.y - (t.scale.y * 0.5f);
    if((p.x > x && p.x < x + t.scale.x)) {
        if((p.y > y && p.y < y + t.scale.y)) {
            return true;
        }
    }

    return false;
}

Bool is_offscreen(Transform t) {
    if((t.pos.x > 1.0f || t.pos.x < 0.0f) || (t.pos.y > 1.0f || t.pos.y < 0.0f)) {
        return true;
    }
    else {
        return false;
    }
}

Void draw_entity_text(sglp_API *api, Game_State *gs, Entity *entity, V2 mouse_position, Transform trans) {
    if(point_overlap(mouse_position, trans)) {
        V2 word_size = v2(0.025f, 0.025f);

        Int buffer_size = 256 * 256;
        sglp_TempMemory tm = api->sglp_push_temp_memory(api, buffer_size);
        Char *buffer = api->sglp_push_off_temp_memory(&tm, buffer_size);
        pp_serialize_struct_type(entity, entity->type, buffer, buffer_size);
        draw_word(buffer, api, gs, mouse_position, word_size);
        api->pop_temp_memory(api, &tm);
    }
}

// TODO - When this function draws text it'd be good if it made sure the text was always onscreen.
Void draw_debug_information(sglp_API *api, Game_State *gs) {
#if INTERNAL
    V2 mouse_position = v2(api->mouse_x, api->mouse_y);

    Entity *next = gs->entity;
    while(next) {
        V2 position_to_draw = {0};
        Transform trans = {0};
        switch(next->type) {
            case pp_Type_Player: trans = next->player.trans; break;
            case pp_Type_Enemy:  trans = next->enemy.trans;  break;
            case pp_Type_Bullet: trans = next->bullet.trans; break;
        }

        draw_entity_text(api, gs, next, mouse_position, trans);

        next = next->next;
    }

#endif
}

void render(sglp_API *api, Game_State *gs) {
    api->clear_screen_for_frame();

    // Render all entities.
    Entity *next = gs->entity;
    while(next) {
        Int current_frame = 0;
        Transform trans = {0};
        Sprite_ID id = Sprite_ID_unknown;

        switch(next->type) {
            case pp_Type_Player: {
                trans = next->player.trans;
                id = Sprite_ID_player;
                current_frame = next->player.current_frame;
            } break;

            case pp_Type_Enemy: {
                trans = next->enemy.trans;
                id = Sprite_ID_enemy_one;
            } break;

            case pp_Type_Bullet: {
                trans = next->bullet.trans;
                id = Sprite_ID_bullet;
            } break;

            default: assert(0); break;
        }

        sglm_Mat4x4 mat = sglm_mat4x4_set_trans_scale_rot(trans.pos.x, trans.pos.y,
                                                          trans.scale.x, trans.scale.y,
                                                          trans.rot);
        Float tform[16] = {0};
        sglm_mat4x4_as_float_arr(tform, &mat);

        api->draw_sprite(gs->sprite[id], current_frame, tform);

        next = next->next;
    }

    // TODO - Read the mouse position from sgl_platform.
    draw_debug_information(api, gs);
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

Bullet create_bullet(void) {
    Bullet res = {0};

    res.trans.pos = v2(400, 400);

    return res;
}

void init(sglp_API *api, Game_State *gs) {

    // Load the player.
    {
        int width, height, number_of_components;
        sglp_File file = api->read_file(api, "player.png");

        uint8_t *img_data = stbi_load_from_memory(file.e, (int)file.size, &width, &height, &number_of_components, 0);
        // TODO - Free file?
        gs->sprite[Sprite_ID_player] = api->load_image(api, img_data, 12, 1, Sprite_ID_player, width, height, number_of_components);
        stbi_image_free(img_data);

        Player player = create_player(0.5f, 0.7f);
        gs->entity = push_entity(api, gs->entity, &player, pp_Type_Player);
    }

    // Load the enemy.
    {
        int width, height, number_of_components;
        sglp_File file = api->read_file(api, "enemy_one.png");
        uint8_t *img_data = stbi_load_from_memory(file.e, (int)file.size, &width, &height, &number_of_components, 0);
        gs->sprite[Sprite_ID_enemy_one] = api->load_image(api, img_data, 8, 1, Sprite_ID_enemy_one, width, height, number_of_components);
        stbi_image_free(img_data);

        Float x = 0.1f;
        for(Int i = 0; (i < 4); ++i) {
            Enemy enemy = create_enemy(x, 0.5f);
            gs->entity = push_entity(api, gs->entity, &enemy, pp_Type_Enemy);

            x += 0.2f;
        }
    }

    // Load font.
    {
        int width, height, number_of_components;
        sglp_File file = api->read_file(api, "freemono.png");
        uint8_t *img_data = stbi_load_from_memory(file.e, (int)file.size, &width, &height, &number_of_components, 0);
        gs->sprite[Sprite_ID_bitmap_font] = api->load_image(api, img_data, 16, 16, Sprite_ID_bitmap_font, width, height, number_of_components);
        stbi_image_free(img_data);
    }

    // Load bullets
    {
        Int width, height, number_of_components;
        sglp_File file = api->read_file(api, "bullet.png");
        uint8_t *img_data = stbi_load_from_memory(file.e, (int)file.size, &width, &height, &number_of_components, 0);
        gs->sprite[Sprite_ID_bullet] = api->load_image(api, img_data, 1, 1, Sprite_ID_bullet, width, height, number_of_components);
        stbi_image_free(img_data);

        for(Int i = 0; (i < 4); ++i) {
            Bullet bullet = create_bullet();
            gs->entity = push_entity(api, gs->entity, &bullet, pp_Type_Bullet);
        }
    }

    // Load background music.
    {
        sglp_File background_wav = api->read_file(api, "music_test.wav");
        Bool success = api->load_wav(api, ID_sound_background, background_wav.e, background_wav.size);
        if(success) {
            api->free_file(api, &background_wav);
            api->play_audio(api, ID_sound_background);
        }
    }

    // Load bloop.
    {
        sglp_File bloop_wav = api->read_file(api, "bloop_00.wav");
        Bool success = api->load_wav(api, ID_sound_bloop, bloop_wav.e, bloop_wav.size);
        if(success) {
            api->free_file(api, &bloop_wav);
        }
    }
}

Void handle_player_movement(Player *player, sglp_API *api) {
    V2 current_speed = player->current_speed;
    V2 max_speed = v2(0.01f, 0.01f);
    V2 acceleration = v2(0.006f, 0.006f);
    V2 friction = v2(0.005f, 0.005f);

    if(api->key['W']) {
        current_speed.y += accelerate(current_speed.y, max_speed.y, acceleration.y * api->dt, false);
        player->dir = Player_Direction_up;
        player->current_frame = Player_Direction_up;
    }
    else if(api->key['S']) {
        current_speed.y += accelerate(current_speed.y, max_speed.y, acceleration.y * api->dt, true);
        player->dir = Player_Direction_down;
        player->current_frame = Player_Direction_down;
    }
    else {
        if(current_speed.y > friction.y * 0.5f) {
            current_speed.y += accelerate(current_speed.y, max_speed.y, friction.y * api->dt, false);
        }
        else if(current_speed.y < -friction.y * 0.5f) {
            current_speed.y += accelerate(current_speed.y, max_speed.y, friction.y * api->dt, true);
        }
        else {
            current_speed.y = 0;
        }
    }

    if(api->key['A']) {
        current_speed.x += accelerate(current_speed.x, max_speed.x, acceleration.x * api->dt, false);
        player->dir = Player_Direction_left;
        player->current_frame = Player_Direction_left;
    }
    else if(api->key['D']) {
        current_speed.x += accelerate(current_speed.x, max_speed.x, acceleration.x * api->dt, true);
        player->dir = Player_Direction_right;
        player->current_frame = Player_Direction_right;
    }
    else {
        if(current_speed.x > friction.x * 0.5f) {
            current_speed.x += accelerate(current_speed.x, max_speed.x, friction.x * api->dt, false);
        }
        else if(current_speed.x < -friction.x * 0.5f) {
            current_speed.x += accelerate(current_speed.x, max_speed.x, friction.x * api->dt, true);
        }
        else {
            current_speed.x = 0;
        }
    }

    player->current_speed = current_speed;
    player->trans.pos.x += player->current_speed.x;
    player->trans.pos.y += player->current_speed.y;
}

Void update_player(Player *player, Entity *player_entity, sglp_API *api, Game_State *gs) {
    handle_player_movement(player, api);

    // Shooting.
    if(api->key[sglp_key_space] && player->can_shoot) {
        player->shot_timer = 200;
        player->can_shoot = false;
        api->play_audio(api, ID_sound_bloop);

        Bullet *bullet = find_first_entity(gs->entity, pp_Type_Bullet);
        bullet->dir = player_direction_to_direction(player->dir);
        bullet->trans = player->trans;
        bullet->parent = player_entity;
    }

    if(!player->can_shoot) {
        player->shot_timer -= api->dt;
        if(player->shot_timer <= 0) {
            player->can_shoot = true;
        }
    }

    // Update frame.
    player->current_frame += 0.5f;
    if(player->current_frame >= player->dir + 2.0f) {
        player->current_frame = player->dir;
    }

    // DEBUG - Rotation
    float rot_speed = 5.0f;
    if(api->key[sglp_key_left])  player->trans.rot += rot_speed;
    if(api->key[sglp_key_right]) player->trans.rot -= rot_speed;

    // DEBUG - scaling
    float scaling_speed = 0.01f;
    if(api->key['I']) player->trans.scale.y += scaling_speed;
    if(api->key['K']) player->trans.scale.y -= scaling_speed;
    if(api->key['J']) player->trans.scale.x -= scaling_speed;
    if(api->key['L']) player->trans.scale.x += scaling_speed;
}

Void update_enemy(Enemy *enemy, Game_State *gs) {
    Player *player = find_first_entity(gs->entity, pp_Type_Player);
    assert(player);

    // Set the player to the start if they touch an enemy.
    if(overlap(player->trans, enemy->trans)) {
        player->trans.pos = player->start_pos;
    }

    // Kill the enemy if they touch the player's bullet.
    Entity *next = gs->entity;
    while(next) {
        if(next->type == pp_Type_Bullet) {
            Bullet *bullet = &next->bullet;
            if(overlap(bullet->trans, enemy->trans)) {
                // next->valid = false; // TODO - Can't set to invalid right now.
                bullet->dir = Direction_unknown;
            }
        }

        next = next->next;
    }
}

void update_bullet(Bullet *bullet, Game_State *gs) {
    // Bullet movement
    // TODO - Make the bullet an entity.
    if(bullet->dir != Direction_unknown) {
        Float bullet_speed = 0.002f;

        if(bullet->dir == Direction_left)       bullet->trans.pos.x -= bullet_speed;
        else if(bullet->dir == Direction_right) bullet->trans.pos.x += bullet_speed;

        if(bullet->dir == Direction_up)         bullet->trans.pos.y -= bullet_speed;
        else if(bullet->dir == Direction_down)  bullet->trans.pos.y += bullet_speed;

        if(is_offscreen(bullet->trans)) {
            bullet->dir = Direction_unknown;
        }
    }

    if(bullet->dir == Direction_unknown) {
        bullet->trans.pos = v2(400, 400);
        Entity *parent = bullet->parent;
        if(parent) {
            switch(parent->type) {
                case pp_Type_Player: parent->player.can_shoot = true; break;

                default: assert(0); break;
            }

            bullet->parent = 0;
        }
    }
}

Void update(sglp_API *api, Game_State *gs) {
    Entity *next = gs->entity;
    while(next) {
        switch(next->type) {
            case pp_Type_Player: update_player(&next->player, next, api, gs); break;
            case pp_Type_Enemy:  update_enemy(&next->enemy, gs);        break;
            case pp_Type_Bullet: update_bullet(&next->bullet, gs);      break;

            default: assert(0); break;
        }

        next = next->next;
    }
}

void sglp_platform_update_and_render_callback(sglp_API *api) {
    global_api = api;

    Game_State *gs = (Game_State *)api->game_state_memory;

    if(api->init_game) {
        init(api, gs);
    }
    else {
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
