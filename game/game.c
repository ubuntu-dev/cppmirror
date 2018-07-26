#define STB_SPRINTF_IMPLEMENTATION
#include "../shared/stb_sprintf.h"

// #define PP_SKIP

#define SGLM_IMPLEMENTATION
#define SGLM_USING_NAMESPACE_SGLM_V2
#include "../shared/sgl_math.h"

#define SGL_IMPLEMENTATION
#define SGL_NO_CRT_DLL
#include "../shared/sgl.h"

#define SGLP_CALLBACK_FILE_LINKAGE SGL_EXPORT
#include "../shared/sgl_platform.h"

#define PP_ASSERT(x) SGL_ASSERT(x)// TODO - Implement.
#define PP_SPRINTF stbsp_snprintf
#include "pp_generated.h"

static sglp_API *global_api;

#define STBI_NO_STDIO
#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#define STBI_ASSERT SGL_ASSERT
#define STBI_MALLOC global_api->sglp_malloc
#define STBI_REALLOC global_api->sglp_realloc
#define STBI_FREE global_api->sglp_free
PP_IGNORE
#include "../shared/stb_image.h"

#define assert(x) SGL_ASSERT(x)

enum Sprite_ID {
    Sprite_ID_unknown,

    Sprite_ID_player,
    Sprite_ID_enemy_one,
    Sprite_ID_bitmap_font,
    Sprite_ID_bullet,
    Sprite_ID_ground,
    Sprite_ID_jumpthrough_ground,
    Sprite_ID_block,
};

struct Transform {
    sglm_V2 position;
    sglm_V2 scale;
    Float rotation;
};
Transform create_transform(sglm_V2 position, sglm_V2 scale, Float rotation) {
    Transform res;
    res.position = position;
    res.scale = scale;
    res.rotation = rotation;

    return(res);
}
enum Direction {
    Direction_unknown,
    Direction_left,
    Direction_right,
    Direction_up,
    Direction_down,
};

struct Bullet {
    Transform transform;
    Direction dir;
    Entity *parent;
};
Bullet bullet(void) {
    Bullet res = {0};
    res.transform.scale.x = 0.025f;
    res.transform.scale.y = 0.025f;

    return (res);
}

// The player direction is a little complicated because it's used for the animation.
enum Player_Direction {
    Player_Direction_left = 0,
    Player_Direction_right = 2,
    Player_Direction_up = 4,
    Player_Direction_down = 6,
};
struct Player {
    Transform transform;
    sglm_V2 start_pos;
    Player_Direction dir;
    Float current_frame;
    sglm_V2 current_speed;

    Bool can_shoot;
    Int shot_timer;
};

Direction player_direction_to_direction(Player_Direction pd) {
    Direction d = Direction_unknown;
    if      (pd == Player_Direction_left)  { d = Direction_left;  }
    else if (pd == Player_Direction_right) { d = Direction_right; }
    else if (pd == Player_Direction_up)    { d = Direction_up;    }
    else if (pd == Player_Direction_down)  { d = Direction_down;  }

    return (d);
}

struct Enemy {
    Transform transform;
};

struct Ground {
    Transform transform;
};

struct JumpThrough_Ground {
    Transform transform;
};

struct Block {
    Transform transform;
    sglm_V2 current_speed;
};

struct TextObject {
    char *str;
    sglm_V2 position;
    sglm_V2 word_size;
};

struct Entity {
    // These have to be first in the struct, so I can cast between them.
    union {
        Player player;
        Enemy enemy;
        Bullet bullet;
        Ground ground;
        JumpThrough_Ground jumpthrough_ground;
        Block block;
        TextObject text;
    };

    pp_Type type;
    Entity *next;
};

struct Room {
    Bool valid;
    sglm_V2 top_left;
    Entity *root;
    Room *up;
    Room *down;
    Room *left;
    Room *right;
};

struct Entity_Info {
    Bool valid;
    Sprite_ID sprite_id;

    sglm_V2 *position;
    sglm_V2 *scale;
    Float *rotation;

    Float *current_frame;
};

Entity_Info get_entity_info(Entity *entity) {
    Entity_Info res = {0};
    switch (entity->type) {
        case pp_Type_Player: {
            Player *x = (Player *)entity;
            res.sprite_id = Sprite_ID_player;
            res.valid = true;

            res.position = &x->transform.position;
            res.scale = &x->transform.scale;
            res.rotation = &x->transform.rotation;

            res.current_frame = &x->current_frame;
        } break;

        case pp_Type_Enemy:  {
            Enemy *x = (Enemy *)entity;
            res.sprite_id = Sprite_ID_enemy_one;
            res.valid = true;

            res.position = &x->transform.position;
            res.scale = &x->transform.scale;
            res.rotation = &x->transform.rotation;
        } break;

        case pp_Type_Bullet: {
            Bullet *x = (Bullet *)entity;
            res.sprite_id = Sprite_ID_bullet;
            res.valid = true;

            res.position = &x->transform.position;
            res.scale = &x->transform.scale;
            res.rotation = &x->transform.rotation;
        } break;

        case pp_Type_Ground: {
            Ground *x = (Ground *)entity;
            res.sprite_id = Sprite_ID_ground;
            res.valid = true;

            res.position = &x->transform.position;
            res.scale = &x->transform.scale;
            res.rotation = &x->transform.rotation;
        } break;

        case pp_Type_JumpThrough_Ground: {
            JumpThrough_Ground *x = (JumpThrough_Ground *)entity;
            res.sprite_id = Sprite_ID_jumpthrough_ground;
            res.valid = true;

            res.position = &x->transform.position;
            res.scale = &x->transform.scale;
            res.rotation = &x->transform.rotation;
        } break;

        case pp_Type_Block: {
            Block *x = (Block *)entity;
            res.sprite_id = Sprite_ID_block;
            res.valid = true;

            res.position = &x->transform.position;
            res.scale = &x->transform.scale;
            res.rotation = &x->transform.rotation;
        } break;

        case pp_Type_TextObject: {
            TextObject *x = (TextObject *)entity;
            res.sprite_id = Sprite_ID_bitmap_font;
            res.valid = true;

            res.position = &x->position;
        } break;
    }

    return (res);
}

Entity *get_end_entity(Entity *root) {
    Entity *next = root;
    if(!root) {
        next = root;
    } else {
        while (next->next) {
            next = next->next;
        }
    }

    return (next);
}

Void add_entity_to_room(sglp_API *api, Room *room, Void *var, pp_Type type) {
    Entity *next = (Entity *)api->sglp_push_permanent_struct(api, Entity);
    uintptr_t size = pp_get_size_from_type(type);

    if(!room->root) {
        room->root = next;
    } else {
        Entity *end = get_end_entity(room->root);
        end->next = next;
    }

    sgl_memcpy(next, var, size);
    next->type = type;

    Entity_Info entity_info = get_entity_info(next);
    assert(entity_info.valid);

    entity_info.position->x += room->top_left.x;
    entity_info.position->y += room->top_left.y;
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
    Void *res = 0;
    if(root) {
        Entity *node = (Entity *)root;
        res = find_first_entity(node->next, type);
    }

    return(res);
}

struct Camera {
    sglm_V2 position;
    sglm_V2 speed;
    // Void *entity_to_follow;
    Bool follow_exactly;
};

#define INPUT_DELAY 100

#define NUMBER_OF_ROOMS 32
struct Game_State {
    sglp_Sprite sprite[pp_get_enum_size_const(Sprite_ID)];

    Room room[NUMBER_OF_ROOMS];
    Room *current_room;

    Entity *free_list; // TODO - Use me.

    Camera camera;

    Direction gravity_direction;

#if INTERNAL
    Bool show_text_box;
    Char *text_box_input;
    Int input_delay;
#endif
};

enum Sound_ID {
    ID_unknown,

    ID_sound_bloop,
    ID_sound_background,
};

SGLP_CALLBACK_FILE_LINKAGE void sglp_platform_setup_settings_callback(sglp_Settings *settings) {
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
    if (res > max) {
        res = max;
    } else if (res < -max) {
        res = -max;
    }

    return (res);
}

sglm_V2 get_letter_position(char Letter);
void draw_word(char const *str, sglp_API *api, Game_State *gs, sglm_V2 position, sglm_V2 scale) {
    scale.x *= 0.5f;
    Int string_length = sgl_string_len(str);
    Float running_x = position.x;
    Float running_y = position.y;
    for (Int i = 0; (i < string_length - 1); ++i) {
        Char letter = str[i];

        if (letter == '\n') {
            running_y += scale.y;
            running_x = position.x;
        } else {
            sglm_V2 pos_in_table = get_letter_position(letter);
            if (pos_in_table.x != -1 && pos_in_table.y != -1) {
                sglm_Mat4x4 mat = sglm_mat4x4_set_trans_scale(running_x, running_y, scale.x, scale.y);
                float tform[16] = {0};

                sglm_mat4x4_as_float_arr(tform, &mat);
                api->sglp_draw_sprite_frame_matrix(api, gs->sprite[Sprite_ID_bitmap_font], pos_in_table.x, pos_in_table.y, tform);

                running_x += scale.x;
            }
        }
    }
}

sglm_V2 get_centre_pos(Transform t) {
    sglm_V2 res;
    res.x = t.position.x - (t.scale.x * 0.5f);
    res.y = t.position.y - (t.scale.y * 0.5f);

    return (res);
}

Direction get_opposite(Direction dir) {
    Direction res = Direction_unknown;
    switch (dir) {
        case Direction_up:    { res = Direction_down;  } break;
        case Direction_down:  { res = Direction_up;    } break;
        case Direction_left:  { res = Direction_right; } break;
        case Direction_right: { res = Direction_left;  } break;

        default: { assert(0); } break;
    }

    return (res);
}

Bool is_vertical(Direction d) {
    assert(d);

    Bool res = false;
    if (d == Direction_up || d == Direction_down) {
        res = true;
    }

    return (res);
}


// This gets the lower 10%.
Transform get_feet(Transform t, Direction dir) {
    Transform res = {0};

    Float foot_size = 0.1f;

    res.scale.x = t.scale.x;
    res.scale.y = t.scale.y;

    res.position.x = t.position.x;
    res.position.y = t.position.y;

    res.rotation = t.rotation;

    if(is_vertical(dir)) { res.scale.y = t.scale.y * foot_size; res.scale.x *= 0.9f; }
    else                 { res.scale.x = t.scale.x * foot_size; res.scale.y *= 0.9f; }

    switch (dir) {
        case Direction_down:  { res.position.y = t.position.y + ((t.scale.y * 0.5f) * (1.0f - foot_size)); } break;
        case Direction_up:    { res.position.y = t.position.y - ((t.scale.y * 0.5f) * (1.0f - foot_size)); } break;
        case Direction_right: { res.position.x = t.position.x + ((t.scale.x * 0.5f) * (1.0f - foot_size)); } break;
        case Direction_left:  { res.position.x = t.position.x - ((t.scale.x * 0.5f) * (1.0f - foot_size)); } break;

        default: { assert(0); } break;
    }

    return (res);
}

// This gets the top 10%.
Transform get_head(Transform t, Direction dir) {
    Transform res = get_feet(t, get_opposite(dir));

    return (res);
}

Bool overlap(Transform a, Transform b) {
    sglm_V2 centre_a = get_centre_pos(a);
    sglm_V2 centre_b = get_centre_pos(b);

    if ((centre_a.x >= centre_b.x && centre_a.x <= centre_b.x + b.scale.x) || (centre_b.x >= centre_a.x && centre_b.x <= centre_a.x + a.scale.x)) {
        if ((centre_a.y >= centre_b.y && centre_a.y <= centre_b.y + b.scale.y) || (centre_b.y >= centre_a.y && centre_b.y <= centre_a.y + a.scale.y)) {
            return true;
        }
    }

    return false;
}

Bool point_overlap(sglm_V2 p, Transform t) {
    Float x = t.position.x - (t.scale.x * 0.5f);
    Float y = t.position.y - (t.scale.y * 0.5f);
    if ((p.x > x && p.x < x + t.scale.x)) {
        if ((p.y > y && p.y < y + t.scale.y)) {
            return true;
        }
    }

    return false;
}

// TODO - Pass just the speed, not the player in here.
Bool is_falling(sglm_V2 current_speed, Direction gravity_direction) {
    Bool res = false;
    Float delta = 0.0f;
    if((gravity_direction == Direction_down && current_speed.y > delta) ||
            (gravity_direction == Direction_up && current_speed.y < -delta) ||
            (gravity_direction == Direction_right && current_speed.x > delta) ||
            (gravity_direction == Direction_left && current_speed.x < -delta)) {
        res = true;
    }

    return (res);
}

Bool is_jumping(sglm_V2 current_speed, Direction gravity_direction) {
    Bool res = is_falling(current_speed, get_opposite(gravity_direction));

    return (res);
}

Bool is_offscreen(Transform t) {
    if((t.position.x > 1.0f || t.position.x < 0.0f) || (t.position.y > 1.0f || t.position.y < 0.0f)) {
        return true;
    } else {
        return false;
    }
}

Void draw_entity_text(sglp_API *api, Game_State *gs, Entity *entity, sglm_V2 mouse_position, sglm_V2 top_left, Transform transform) {
    sglm_V2 mouse_position_in_world = sglm_v2(mouse_position.x + top_left.x, mouse_position.y + top_left.y);

    if(point_overlap(mouse_position_in_world, transform)) {
        sglm_V2 word_size = sglm_v2(0.025f, 0.025f);

        Int buffer_size = 256 * 256;
        sglp_TempMemory tm = api->sglp_push_temp_memory(api, buffer_size);
        Char *buffer = (Char *)api->sglp_push_off_temp_memory(&tm, buffer_size);
        pp_serialize_struct_type(entity, entity->type, buffer, buffer_size);

        draw_word(buffer, api, gs, mouse_position, word_size);
        api->sglp_pop_temp_memory(api, &tm);
    }
}

Void draw_block(sglp_API *api, Game_State *gs, Transform transform) {
    Float x = transform.position.x - gs->camera.position.x;
    Float y = transform.position.y - gs->camera.position.y;
    sglm_Mat4x4 mat = sglm_mat4x4_set_trans_scale_rot(x, y,
                                                      transform.scale.x, transform.scale.y,
                                                      transform.rotation);

    Float tform[16] = {0};
    sglm_mat4x4_as_float_arr(tform, &mat);

    api->sglp_draw_black_box(api, tform);
}

// TODO - When this function draws text it'd be good if it made sure the text was always onscreen.
// TODO - Be nice to have some way to display the player with a keypress.
Void draw_debug_information(sglp_API *api, Game_State *gs) {
#if INTERNAL

    Player *player = (Player *)find_first_entity(gs->current_room->root, pp_Type_Player);

    // TODO - This is actually kinda awful (and doesn't work).
    if (gs->show_text_box) {
        sglm_V2 size = sglm_v2(1.0f, 0.25f);
        sglm_Mat4x4 mat = sglm_mat4x4_set_trans_scale_rot(0.0f + (size.x * 0.5f), 0.0f + (size.y * 0.5f),
                                                          size.x, size.y,
                                                          0.0f);
        Float tform[16] = {0};
        sglm_mat4x4_as_float_arr(tform, &mat);

        api->sglp_draw_black_box(api, tform);


        sglm_V2 word_size = sglm_v2(0.025f, 0.025f);
        draw_word(gs->text_box_input, api, gs, sglm_v2(0.1f, 0.1f), word_size);
    }

    sglm_V2 mouse_position = sglm_v2(api->mouse_x, api->mouse_y);

    if(1) {
        Int buffer_size = 256 * 256;
        sglp_TempMemory temp_memory = api->sglp_push_temp_memory(api, buffer_size);
        Char *buffer = (Char *)api->sglp_push_off_temp_memory(&temp_memory, buffer_size);
        pp_serialize_struct(&mouse_position, sglm_V2, buffer, buffer_size);
        draw_word(buffer, api, gs, sglm_v2(0.5f, 0.5f), sglm_v2(0.025f, 0.025f));
        api->sglp_pop_temp_memory(api, &temp_memory);
    }

    Entity *next = gs->current_room->root;
    while(next) {
        sglm_V2 position_to_draw = {0};
        Transform transform = {0};
        switch(next->type) {
            case pp_Type_Player: { transform = next->player.transform; } break;
            case pp_Type_Enemy:  { transform = next->enemy.transform;  } break;
            case pp_Type_Bullet: { transform = next->bullet.transform; } break;
        }

        draw_entity_text(api, gs, next, mouse_position, gs->current_room->top_left, transform);

        next = next->next;
    }

    // Draw a box around the players feet.
    if(1) {
        draw_block(api, gs, get_feet(player->transform, gs->gravity_direction));
    }

    if(0) {
        Char *s = is_falling(player->current_speed, gs->gravity_direction) ? "Falling" : "Jumping";
        draw_word(s, api, gs, sglm_v2(0.5f, 0.5f), sglm_v2(0.025f, 0.025f));
    }

#endif
}

void render_room(sglp_API *api, Game_State *gs, Room *room) {
    if(room) {
        Entity *next = room->root;
        while(next) {
            if(next->type == pp_Type_TextObject) {
                TextObject *text = &next->text;
                Float x = text->position.x - gs->camera.position.x;
                Float y = text->position.y - gs->camera.position.y;
                draw_word(text->str, api, gs, sglm_v2(x, y), text->word_size);
            } else {
                Entity_Info entity_info = get_entity_info(next);
                assert(entity_info.valid);

                if(entity_info.position && entity_info.scale && entity_info.rotation) {
                    Float x = entity_info.position->x - gs->camera.position.x;
                    Float y = entity_info.position->y - gs->camera.position.y;
                    sglm_Mat4x4 mat = sglm_mat4x4_set_trans_scale_rot(x, y,
                                                                      entity_info.scale->x, entity_info.scale->y,
                                                                      *entity_info.rotation);
                    Float tform[16] = {0};
                    sglm_mat4x4_as_float_arr(tform, &mat);

                    Int current_frame = (entity_info.current_frame) ? sgl_floor_float_to_int(*entity_info.current_frame) : 0;
                    api->sglp_draw_sprite(api, gs->sprite[entity_info.sprite_id], current_frame, tform);
                }
            }

            next = next->next;
        }
    }
}

void render(sglp_API *api, Game_State *gs) {
    api->sglp_clear_screen_for_frame(api);

    // Render all entities.
    // TODO - Render all room above/below and left/right? For transitions?

    render_room(api, gs, gs->current_room);

    render_room(api, gs, gs->current_room->up);
    render_room(api, gs, gs->current_room->down);
    render_room(api, gs, gs->current_room->left);
    render_room(api, gs, gs->current_room->right);

    // TODO - Read the mouse position from sgl_platform.
    draw_debug_information(api, gs);
}

Enemy create_enemy(float x, float y) {
    Enemy res = {0};

    res.transform.scale.x = 0.1f;
    res.transform.scale.y = 0.1f;

    res.transform.position.x = x;
    res.transform.position.y = y;

    return (res);
}

Player create_player(float x, float y) {
    Player res = {0};

    res.transform.scale.x = 0.1f;
    res.transform.scale.y = 0.1f;

    res.transform.position.x = x;
    res.transform.position.y = y;

    res.start_pos = res.transform.position;
    res.dir = Player_Direction_left;

    return (res);
}

Bullet create_bullet(void) {
    Bullet res = {0};

    res.transform.position = sglm_v2(400, 400);

    return (res);
}

// TODO - Right now this takes a centre position and a x/y size from that. I'd prefer it take a top-left position and width/height from there.
Ground create_ground(Float x, Float y) {
    Ground res = {0};

    //res.transform.position = sglm_v2(x, y);
    //res.transform.scale = sglm_v2(0.1f, 0.1f);

    Float scalex = 0.1f;
    Float scaley = 0.1f;
    res.transform.position = sglm_v2(x + (scalex * 0.5f), y + (scaley * 0.5f));
    res.transform.scale = sglm_v2(scalex, scaley);

    return (res);
}

JumpThrough_Ground create_jump_through_ground(Float x, Float y, Float scalex, Float scaley) {
    JumpThrough_Ground res = {0};

    res.transform.position = sglm_v2(x - (scalex * 0.5f), y - (scaley * 0.5f));
    res.transform.scale = sglm_v2(scalex, scaley);

    return(res);
}

Block create_block(Float x, Float y) {
    Block res = {0};

    res.transform.position = sglm_v2(x, y);
    res.transform.scale = sglm_v2(0.1f, 0.1f);

    return(res);
}

void load_image(sglp_API *api, sglp_Sprite *sprite, char const *fname, Sprite_ID id, int frame_count_x, int frame_count_y) {
    Int width, height, number_of_components;
    sglp_File file = api->sglp_read_file(api, fname);
    uint8_t *img_data = stbi_load_from_memory(file.e, (Int)file.size, &width, &height, &number_of_components, 0);
    sprite[id] = api->sglp_load_image(api, img_data, frame_count_x, frame_count_y, id, width, height, number_of_components);
    stbi_image_free(img_data);
}

Char *push_and_copy_string(sglp_API *api, Char *string) {
    Int length = sgl_string_len(string);
    Char *new_string = (Char *)api->sglp_push_permanent_memory(api, length);
    sgl_memcpy(new_string, string, length);

    return(new_string);
}

TextObject create_text(sglp_API *api, Char *string, sglm_V2 position, sglm_V2 word_size) {
    TextObject res = {0};

    // TODO - It'd be cool if I manually inserted newlines in here if the word was going offscreen (crossed over
    //        the whole number line).
    res.str = push_and_copy_string(api, string);
    res.position = position;
    res.word_size = word_size;

    return(res);
}

Void create_room(sglp_API *api, Room *room, sglm_V2 top_left, Char *format, Int len) {
    assert(sgl_string_len(format) == 100);

    room->top_left = top_left;
    room->valid = true;

    Int width = 10;
    Int height = 10;

    for(Int i = 0; (i < width); ++i) {
        for(Int j = 0; (j < height); ++j) {
            Float x = (Float)j * 0.1f;
            Float y = (Float)i * 0.1f;

            Char c = format[(i * width) + j];
            switch(c) {
                case 'p': {
                    Player ent = create_player(x, y);
                    add_entity_to_room(api, room, &ent, pp_Type_Player);
                } break;

                case 'x': {
                    Ground ent = create_ground(x, y);
                    add_entity_to_room(api, room, &ent, pp_Type_Ground);
                } break;
            }
        }
    }
}

Void stitch_rooms(Game_State *gs) {
    for(Int i = 0; (i < NUMBER_OF_ROOMS); ++i) {
        if(gs->room[i].valid) {
            Int current_x = sgl_floor_float_to_int(gs->room[i].top_left.x);
            Int current_y = sgl_floor_float_to_int(gs->room[i].top_left.y);

            for(Int j = 0; (j < NUMBER_OF_ROOMS); ++j) {
                if(gs->room[j].valid) {
                    Int next_x = sgl_floor_float_to_int(gs->room[j].top_left.x);
                    Int next_y = sgl_floor_float_to_int(gs->room[j].top_left.y);

                    if     (current_x + 1 == next_x && current_y == next_y) { gs->room[i].right = &gs->room[j]; }
                    else if(current_x - 1 == next_x && current_y == next_y) { gs->room[i].left  = &gs->room[j]; }
                    else if(current_y + 1 == next_y && current_x == next_x) { gs->room[i].down  = &gs->room[j]; }
                    else if(current_y - 1 == next_y && current_x == next_x) { gs->room[i].up    = &gs->room[j]; }
                }
            }

            if(!gs->room[i].right || !gs->room[i].left || !gs->room[i].down || !gs->room[i].up) {
                // The room is floating in the middle of nowhere!
                assert(0);
            }
        }
    }
}

Void init(sglp_API *api, Game_State *gs) {

    load_image(api, gs->sprite, "player.png",    Sprite_ID_player,             12,  1);
    load_image(api, gs->sprite, "enemy_one.png", Sprite_ID_enemy_one,           8,  1);
    load_image(api, gs->sprite, "freemono.png",  Sprite_ID_bitmap_font,        16, 16);
    load_image(api, gs->sprite, "bullet.png",    Sprite_ID_bullet,              1,  1);
    load_image(api, gs->sprite, "block.png",     Sprite_ID_ground,              1,  1);
    load_image(api, gs->sprite, "block.png",     Sprite_ID_jumpthrough_ground,  1,  1);
    load_image(api, gs->sprite, "block.png",     Sprite_ID_block,               1,  1);

    Int room_index = 0;

    Char *room_one =
        "          "
        "          "
        "          "
        "          "
        "          "
        "          "
        "     p    "
        "          "
        "xxxxxxxxxx"
        "          ";
    create_room(api, &gs->room[room_index++], sglm_v2(0.0f, 0.0f), room_one, sgl_string_len(room_one));

    Char *room_two =
        "          "
        "          "
        "          "
        "          "
        "          "
        "          "
        "    x     "
        "    x     "
        "xxxxx     "
        "          ";
    create_room(api, &gs->room[room_index++], sglm_v2(1.0f, 0.0f), room_two, sgl_string_len(room_two));

    Char *room_three =
        "          "
        "          "
        "          "
        "xxxxx     "
        "x         "
        "xxxx      "
        "x         "
        "xxxxx     "
        "    xxxxxx"
        "          ";
    create_room(api, &gs->room[room_index++], sglm_v2(1.0f, 1.0f), room_three, sgl_string_len(room_three));

    gs->current_room = &gs->room[0];
    stitch_rooms(gs);


    // Text
#if 0
    TextObject text = create_text(api, "Use the left and right arrows\nto move left and right.", sglm_v2(0.1f, 0.1f), sglm_v2(0.05f, 0.05f));
    push_entity(api, gs->current_room, &text, pp_Type_TextObject);
#endif

    // Camera
    {
        Player *player = (Player *)find_first_entity(gs->current_room->root, pp_Type_Player);
        assert(player);

        gs->camera.speed = sglm_v2(0.05f, 0.05f);
        // gs->camera.entity_to_follow = player;
        gs->camera.follow_exactly = false;
        gs->camera.position.x = (int)player->transform.position.x;
        gs->camera.position.y = (int)player->transform.position.y;
    }

    // Load background music.
    {
        sglp_File background_wav = api->sglp_read_file(api, "music_test.wav");
        Bool success = api->sglp_load_wav(api, ID_sound_background, background_wav.e, background_wav.size);
        if (success) {
            api->sglp_free_file(api, &background_wav);
            api->sglp_play_audio(api, ID_sound_background);
        }
    }

    // Load bloop.
    {
        sglp_File bloop_wav = api->sglp_read_file(api, "bloop_00.wav");
        Bool success = api->sglp_load_wav(api, ID_sound_bloop, bloop_wav.e, bloop_wav.size);
        if (success) {
            api->sglp_free_file(api, &bloop_wav);
        }
    }

    // Gravity
    gs->gravity_direction = Direction_down;

#if INTERNAL
    gs->text_box_input = (Char *)api->sglp_push_permanent_memory(api, 256 * 256);
#endif
}

// TODO - These are awful.
Bool is_solid(Entity *entity) {
    Bool res = false;
    if (entity->type == pp_Type_Ground || entity->type == pp_Type_JumpThrough_Ground || entity->type == pp_Type_Block) {
        res = true;
    }

    return (res);
}

Bool is_non_jumpthrough_block(Entity *entity) {
    Bool res = false;
    if(entity->type == pp_Type_Ground || entity->type == pp_Type_Block) {
        res = true;
    }

    return (res);
}

Void accelerate_in_direction(sglm_V2 *current_speed, sglm_V2 max_speed, Float acceleration, Bool forward, Direction dir) {
    switch(dir) {
        case Direction_down:  { current_speed->y += accelerate(current_speed->y, max_speed.y, acceleration, forward);  } break;
        case Direction_up:    { current_speed->y += accelerate(current_speed->y, max_speed.y, acceleration, !forward); } break;
        case Direction_right: { current_speed->x += accelerate(current_speed->x, max_speed.x, acceleration, forward);  } break;
        case Direction_left:  { current_speed->x += accelerate(current_speed->x, max_speed.x, acceleration, !forward); } break;

        default: { assert(0); } break;
    }
}

// TODO - This modifies the transform. Might be better to return a new struct with the new transform/entity standing on.
Entity *handling_landing_on_block_internal(Entity *next, Transform *transform, sglm_V2 current_speed, Direction gravity_direction) {
    Entity *standing_on = 0;

    Transform copy_transform = *transform;
    if(is_vertical(gravity_direction)) { copy_transform.position.y += current_speed.y; }
    else                               { copy_transform.position.x += current_speed.x; }

    while(next) {
        if(is_solid(next)) {
            Entity_Info entity_info = get_entity_info(next);
            assert(entity_info.valid && entity_info.position && entity_info.scale && entity_info.rotation);

            Transform ground_transform = create_transform(*entity_info.position, *entity_info.scale, *entity_info.rotation);

            if(overlap(get_feet(copy_transform, gravity_direction), get_head(ground_transform, gravity_direction))) {
                switch(gravity_direction) {
                    case Direction_down:  { transform->position.y = ground_transform.position.y - (ground_transform.scale.y * 0.5f) - (transform->scale.y * 0.5f) - current_speed.y; } break;
                    case Direction_up:    { transform->position.y = ground_transform.position.y + (ground_transform.scale.y * 0.5f) + (transform->scale.y * 0.5f) - current_speed.y; } break;
                    case Direction_right: { transform->position.x = ground_transform.position.x - (ground_transform.scale.x * 0.5f) - (transform->scale.x * 0.5f) - current_speed.x; } break;
                    case Direction_left:  { transform->position.x = ground_transform.position.x + (ground_transform.scale.x * 0.5f) + (transform->scale.x * 0.5f) - current_speed.x; } break;

                    default: { assert(0); } break;
                }

                standing_on = next;
                break; // while
            }
        }

        next = next->next;
    }

    return(standing_on);
}

Entity *handle_landing_on_block(Room *room, Transform *transform, sglm_V2 current_speed, Direction gravity_direction) {
    Entity *standing_on = 0;
    if(is_falling(current_speed, gravity_direction)) {

        standing_on = handling_landing_on_block_internal(room->root, transform, current_speed, gravity_direction);

#define HANDLE_DIRECTION(xx) if(!standing_on && room->xx) { standing_on = handling_landing_on_block_internal(room->xx->root, transform, current_speed, gravity_direction); }

        HANDLE_DIRECTION(up)
        HANDLE_DIRECTION(down)
        HANDLE_DIRECTION(left)
        HANDLE_DIRECTION(right)

#undef HANDLE_DIRECTION
    }

    return(standing_on);
}

Bool handle_booping_head_on_block_internal(Entity *next, Transform transform) {
    Bool hit = false;
    while(next) {
        if(is_non_jumpthrough_block(next)) {
            Entity_Info entity_info = get_entity_info(next);
            assert(entity_info.valid && entity_info.position && entity_info.scale && entity_info.rotation);

            if(overlap(transform, create_transform(*entity_info.position, *entity_info.scale, *entity_info.rotation))) {
                hit = true;
                break;
            }
        }

        next = next->next;
    }

    return(hit);
}


Bool handle_booping_head_on_block(Room *room, Transform transform, sglm_V2 current_speed, Direction gravity_direction) {
    Bool hit = false;
    if(is_jumping(current_speed, gravity_direction)) {
        if(is_vertical(gravity_direction)) { transform.position.y += current_speed.y; }
        else                               { transform.position.x += current_speed.x; }

        hit = handle_booping_head_on_block_internal(room->root, transform);

#define HANDLE_DIRECTION(xx) if(!hit && room->xx) { handle_booping_head_on_block_internal(room->xx->root, transform); }

        HANDLE_DIRECTION(up)
        HANDLE_DIRECTION(down)
        HANDLE_DIRECTION(left)
        HANDLE_DIRECTION(right)

#undef HANDLE_DIRECTION
    }

    return(hit);
}

Bool handle_hitting_side_of_block_internal(Entity *next, Transform transform) {
    Bool hit = false;

    while(next) {
        if(is_non_jumpthrough_block(next)) {
            Entity_Info entity_info = get_entity_info(next);
            assert(entity_info.valid && entity_info.position && entity_info.scale && entity_info.rotation);

            if(overlap(transform, create_transform(*entity_info.position, *entity_info.scale, *entity_info.rotation))) {
                hit = true;
                break;
            }
        }

        next = next->next;
    }

    return(hit);
}

Bool handle_hitting_side_of_block(Room *room, Transform transform, sglm_V2 current_speed, Direction gravity_direction) {
    // Handle hitting the sides of blocks.

    if(is_vertical(gravity_direction)) { transform.position.x += current_speed.x; }
    else                               { transform.position.y += current_speed.y; }

    Bool hit = handle_hitting_side_of_block_internal(room->root, transform);

#define HANDLE_DIRECTION(xx) if(!hit && room->xx) { hit = handle_hitting_side_of_block_internal(room->xx->root, transform); }

    HANDLE_DIRECTION(up)
    HANDLE_DIRECTION(down)
    HANDLE_DIRECTION(left)
    HANDLE_DIRECTION(right)

#undef HANDLE_DIRECTION

    return(hit);
}

Void update_block(Block *block, Game_State *gs, sglp_API *api) {
    sglm_V2 current_speed = block->current_speed;
    sglm_V2 max_speed = sglm_v2(0.01f, 0.01f);
    Float gravity = 0.004f;
    Bool is_gravity_vertical = is_vertical(gs->gravity_direction);

    // Handle landing on a block.
    Entity *entity_standing_on = handle_landing_on_block(gs->current_room, &block->transform, current_speed, gs->gravity_direction);
    if(entity_standing_on) {
        current_speed.x = 0.0f;
        current_speed.y = 0.0f;
    }

    // Gravity.
    accelerate_in_direction(&current_speed, max_speed, gravity * api->dt, true, gs->gravity_direction);

    if(handle_hitting_side_of_block(gs->current_room, block->transform, current_speed, gs->gravity_direction)) {
        if(is_gravity_vertical) { current_speed.x = 0.0f; }
        else                    { current_speed.y = 0.0f; }
    }

    // TODO - What should happen if a block crushes the player?

    block->current_speed.x = current_speed.x;
    block->current_speed.y = current_speed.y;

    block->transform.position.x += block->current_speed.x;
    block->transform.position.y += block->current_speed.y;
}

// TODO - I do a lot of 'overlap' collisions in here, which could fail if something is travelling really fast. I should
//        protect against this?
Void handle_player_movement(Player *player, sglp_API *api, Room *room, Direction gravity_direction) {
    sglm_V2 current_speed = player->current_speed;
    sglm_V2 max_speed = sglm_v2(0.003f, 0.003f);
    sglm_V2 acceleration = sglm_v2(0.002f, 0.002f);
    sglm_V2 friction = sglm_v2(0.005f, 0.005f);
    Float gravity = 0.002f;
    Float jump_power = 0.0015f;

    Bool is_gravity_vertical = is_vertical(gravity_direction);

    Entity *block_standing_on = handle_landing_on_block(room, &player->transform, current_speed, gravity_direction);
    if(handle_booping_head_on_block(room, player->transform, current_speed, gravity_direction)) {
        if(is_gravity_vertical) { current_speed.y = 0.0f; }
        else                    { current_speed.x = 0.0f; }
    }

    // Gravity.
    accelerate_in_direction(&current_speed, max_speed, gravity * api->dt, true, gravity_direction);

    if(block_standing_on) {
        Float falling_delta = 0.001f;
        switch(gravity_direction) {
            case Direction_down:  { if (current_speed.y > 0.0f) { current_speed.y =  falling_delta; } } break;
            case Direction_up:    { if (current_speed.y < 0.0f) { current_speed.y = -falling_delta; } } break;
            case Direction_right: { if (current_speed.x > 0.0f) { current_speed.x =  falling_delta; } } break;
            case Direction_left:  { if (current_speed.x < 0.0f) { current_speed.x = -falling_delta; } } break;

            default: { assert(0); } break;
        }

        // Handle jumping.
        if(api->key[sglp_key_space] ||
                (is_gravity_vertical && (api->key[sglp_key_up] || api->key[sglp_key_down])) ||
                (!is_gravity_vertical && (api->key[sglp_key_right] || api->key[sglp_key_left]))) {
            switch(gravity_direction) {
                case Direction_down:  { current_speed.y -= jump_power * api->dt; } break;
                case Direction_up:    { current_speed.y += jump_power * api->dt; } break;
                case Direction_right: { current_speed.x -= jump_power * api->dt; } break;
                case Direction_left:  { current_speed.x += jump_power * api->dt; } break;

                default: { assert(0); } break;
            }
        }
    }

    if(is_gravity_vertical) {
        if(api->key[sglp_key_left]) {
            current_speed.x += accelerate(current_speed.x, max_speed.x, acceleration.x * api->dt, false);
            player->dir = Player_Direction_left;
            player->current_frame = Player_Direction_left;
        } else if(api->key[sglp_key_right]) {
            current_speed.x += accelerate(current_speed.x, max_speed.x, acceleration.x * api->dt, true);
            player->dir = Player_Direction_right;
            player->current_frame = Player_Direction_right;
        } else {
            if(current_speed.x > friction.x * 0.5f) {
                current_speed.x += accelerate(current_speed.x, max_speed.x, friction.x * api->dt, false);
            } else if(current_speed.x < -friction.x * 0.5f) {
                current_speed.x += accelerate(current_speed.x, max_speed.x, friction.x * api->dt, true);
            } else {
                current_speed.x = 0;
            }
        }
    } else {
        if(api->key[sglp_key_up]) {
            current_speed.y += accelerate(current_speed.y, max_speed.y, acceleration.y * api->dt, false);
            player->dir = Player_Direction_left;
            player->current_frame = Player_Direction_left;
        } else if(api->key[sglp_key_down]) {
            current_speed.y += accelerate(current_speed.y, max_speed.y, acceleration.y * api->dt, true);
            player->dir = Player_Direction_right;
            player->current_frame = Player_Direction_right;
        } else {
            if(current_speed.y > friction.y * 0.5f) {
                current_speed.y += accelerate(current_speed.y, max_speed.y, friction.y * api->dt, false);
            } else if(current_speed.y < -friction.y * 0.5f) {
                current_speed.y += accelerate(current_speed.y, max_speed.y, friction.y * api->dt, true);
            } else {
                current_speed.y = 0;
            }
        }
    }

    if(handle_hitting_side_of_block(room, player->transform, current_speed, gravity_direction)) {
        if(is_gravity_vertical) { current_speed.x = 0.0f; }
        else                    { current_speed.y = 0.0f; }
    }

    player->current_speed.x = current_speed.x;
    player->current_speed.y = current_speed.y;

    player->transform.position.x += player->current_speed.x;
    player->transform.position.y += player->current_speed.y;

}

Void move_entity_to_new_room(Void *var, Room *old_room, Room *new_room) {
    Entity *target = (Entity *)var;

    // Find one before the target;
    Entity *one_before_target = 0;
    {
        Entity *next = old_room->root;
        while(next) {

            if(next->next == target) {
                one_before_target = next;
            }

            next = next->next;
        }
    }

    if(one_before_target) {
        // If we found one before the target then set it to "skip" the target.
        one_before_target->next = target->next;
    } else {
        // Otherwise assume the target was the room and set the new room to target -> next.
        old_room->root = target->next;
    }

    target->next = 0; // Clear the target's next.

    Entity *end = get_end_entity(new_room->root);
    end->next = target;

#if 0
    Entity_Info entity_info = get_entity_info(target);
    if(entity_info.valid) {
        Float diff_x = new_room->top_left.x - old_room->top_left.x;
        Float diff_y = new_room->top_left.y - old_room->top_left.y;

        entity_info.transform->position.x -= diff_x;
        entity_info.transform->position.y -= diff_y;
    }
#endif
}

Void update_player(Player *player, sglp_API *api, Game_State *gs) {
    handle_player_movement(player, api, gs->current_room, gs->gravity_direction);

    // TODO - If the player goes to a invalid "room", it means they've fallen off the map. So handle that case.
    if(player->transform.position.x < gs->current_room->top_left.x) {
        move_entity_to_new_room(player, gs->current_room, gs->current_room->left);
        gs->current_room = gs->current_room->left;
    } else if(player->transform.position.x > gs->current_room->top_left.x + 1.0f) {
        move_entity_to_new_room(player, gs->current_room, gs->current_room->right);
        gs->current_room = gs->current_room->right;
    }

    if(player->transform.position.y < gs->current_room->top_left.y) {
        move_entity_to_new_room(player, gs->current_room, gs->current_room->up);
        gs->current_room = gs->current_room->up;
    } else if(player->transform.position.y > gs->current_room->top_left.y + 1.0f) {
        move_entity_to_new_room(player, gs->current_room, gs->current_room->down);
        gs->current_room = gs->current_room->down;
    }

    // Shooting.
#if 0
    if (api->key['Z'] && player->can_shoot) {
        player->shot_timer = 200;
        player->can_shoot = false;
        api->sglp_play_audio(api, ID_sound_bloop);

        Bullet *bullet = find_first_entity(gs->current_room.root, pp_Type_Bullet);
        bullet->dir = player_direction_to_direction(player->dir);
        bullet->transform = player->transform;
        bullet->parent = (Entity *)player;
    }

    if (!player->can_shoot) {
        player->shot_timer -= api->dt;
        if (player->shot_timer <= 0) {
            player->can_shoot = true;
        }
    }
#endif

    // Update frame.
    player->current_frame += 0.5f;
    if (player->current_frame >= player->dir + 2.0f) {
        player->current_frame = player->dir;
    }

    // DEBUG - Rotation
#if 0
    float rot_speed = 5.0f;
    if (api->key[sglp_key_left])  { player->transform.rotation += rot_speed; }
    if (api->key[sglp_key_right]) { player->transform.rotation -= rot_speed; }

    // DEBUG - scaling
    float scaling_speed = 0.01f;
    if (api->key['I']) { player->transform.scale.y += scaling_speed; }
    if (api->key['K']) { player->transform.scale.y -= scaling_speed; }
    if (api->key['J']) { player->transform.scale.x -= scaling_speed; }
    if (api->key['L']) { player->transform.scale.x += scaling_speed; }
#endif
}

Void update_enemy(Enemy *enemy, Game_State *gs) {
    Player *player = (Player *)find_first_entity(gs->current_room->root, pp_Type_Player);
    if (player) {
        // Set the player to the start if they touch an enemy.
        if (overlap(player->transform, enemy->transform)) {
            // TODO - Reset the players speed when they die.
            player->transform.position = player->start_pos;
        }

        // Kill the enemy if they touch the player's bullet.
        Entity *next = gs->current_room->root;
        while (next) {
            if (next->type == pp_Type_Bullet) {
                Bullet *bullet = &next->bullet;
                if (overlap(bullet->transform, enemy->transform)) {
                    // next->valid = false; // TODO - Can't set to invalid right now.
                    bullet->dir = Direction_unknown;
                }
            }

            next = next->next;
        }
    } else {
        assert(0);
    }
}

void update_bullet(Bullet *bullet, Game_State *gs) {
    // Bullet movement
    if(bullet->dir != Direction_unknown) {
        Float bullet_speed = 0.002f;

        if(bullet->dir == Direction_left)       { bullet->transform.position.x -= bullet_speed; }
        else if(bullet->dir == Direction_right) { bullet->transform.position.x += bullet_speed; }

        if     (bullet->dir == Direction_up)   { bullet->transform.position.y -= bullet_speed; }
        else if(bullet->dir == Direction_down) { bullet->transform.position.y += bullet_speed; }

        if (is_offscreen(bullet->transform)) {
            bullet->dir = Direction_unknown;
        }
    }

    if(bullet->dir == Direction_unknown) {
        bullet->transform.position = sglm_v2(400, 400);
        Entity *parent = bullet->parent;
        if(parent) {
            switch (parent->type) {
                case pp_Type_Player: { parent->player.can_shoot = true; } break;

                default: assert(0); break;
            }

            bullet->parent = 0;
        }
    }
}

Void handle_input_commands(Char *cmd, sglp_API *api, Game_State *gs) {
    /*if(sgl_string_comp(cmd, "HIDEPLAYER")) {
        gs->hide_player = !gs->hide_player;
    }
    else*/ {
        pp_Type gs_type = pp_Type_Game_State;
        Uintptr number_of_gs_members = pp_get_number_of_members(gs_type);
        for (Uintptr i = 0; (i < number_of_gs_members); ++i) {
            pp_MemberDefinition mem = pp_get_members_from_type(gs_type, i);
            if (sgl_string_comp(cmd, mem.name)) {
                // TODO - Do something here? Like display the value?
            }
        }
    }
}

Void update_camera(Game_State *gs) {
    Camera *camera = &gs->camera;
    Player *player = (Player *)find_first_entity(gs->current_room->root, pp_Type_Player);

    // TODO - This doesn't work for negative player positions.
    Float target_x = (float)((int)player->transform.position.x);
    Float target_y = (float)((int)player->transform.position.y);

    if     (camera->position.x < target_x - camera->speed.x) { camera->position.x += camera->speed.x; }
    else if(camera->position.x > target_x + camera->speed.x) { camera->position.x -= camera->speed.x; }
    else                                                     { camera->position.x = target_x;         }

    if(camera->position.y < target_y - camera->speed.y)      { camera->position.y += camera->speed.y; }
    else if(camera->position.y > target_y + camera->speed.y) { camera->position.y -= camera->speed.y; }
    else                                                     { camera->position.y = target_y;         }
}

Void update_room(sglp_API *api, Game_State *gs, Room *room) {
    if(room) {
        Entity *next = gs->current_room->root;
        while(next) {
            switch (next->type) {
                case pp_Type_Player: { update_player(&next->player, api, gs); } break;
                case pp_Type_Enemy:  { update_enemy(&next->enemy, gs);        } break;
                case pp_Type_Bullet: { update_bullet(&next->bullet, gs);      } break;
                case pp_Type_Block:  { update_block(&next->block, gs, api);   } break;

                default: { assert(0); } break;
            }

            next = next->next;
        }
    }
}

Void update(sglp_API *api, Game_State *gs) {

    // Change gravity direction
    if (api->key['W'])      { gs->gravity_direction = Direction_up;    }
    else if (api->key['S']) { gs->gravity_direction = Direction_down;  }
    else if (api->key['A']) { gs->gravity_direction = Direction_left;  }
    else if (api->key['D']) { gs->gravity_direction = Direction_right; }

#if INTERNAL
    if (api->key[sglp_key_ctrl] && api->key[sglp_key_space]) {
        gs->show_text_box = true;
    }
    if (api->key[sglp_key_escape]) { // TODO - Change to "enter" once sgl_platform.h supports it.
        gs->show_text_box = false;

        handle_input_commands(gs->text_box_input, api, gs);

        sgl_zero(gs->text_box_input, sgl_string_len(gs->text_box_input));
    }
#endif

#if INTERNAL
    if (!gs->show_text_box) {
#endif
        update_camera(gs);

        // TODO - I need to take into account a room's position when updating here. May get tricky...
        update_room(api, gs, gs->current_room);
        update_room(api, gs, gs->current_room->up);
        update_room(api, gs, gs->current_room->down);
        update_room(api, gs, gs->current_room->left);
        update_room(api, gs, gs->current_room->right);

#if INTERNAL
    } else {
        // TODO - This won't let you delete letters or put in spaces.
        if(gs->input_delay >= 0) {
            gs->input_delay -= api->dt;
        }

        for(Int/*sglp_Key*/ key = sglp_key_a; key <= sglp_key_z; ++key) {
            Bool val = api->key[key];
            if(val) {
                Int end = sgl_string_len(gs->text_box_input) - 1;

                if(end == 0 || (key != gs->text_box_input[end - 1] || gs->input_delay <= 0)) {
                    gs->text_box_input[end] = key;
                    gs->input_delay = INPUT_DELAY;
                }
            }
        }
    }
#endif
}

SGLP_CALLBACK_FILE_LINKAGE void sglp_platform_update_and_render_callback(sglp_API *api) {
    global_api = api;

    Game_State *gs = (Game_State *)api->game_state_memory;

    if(api->init_game) {
        init(api, gs);
    } else {
        update(api, gs);
        render(api, gs);
    }
}

sglm_V2 get_letter_position(char letter) {
    sglm_V2 res = sglm_v2(-1.0f, -1.0f);
    switch(letter) {
        case ' ': { res = sglm_v2(0.0f, 0.0f);  } break;
        case '!': { res = sglm_v2(1.0f, 0.0f);  } break;
        case '"': { res = sglm_v2(2.0f, 0.0f);  } break;
        case '#': { res = sglm_v2(3.0f, 0.0f);  } break;
        case '$': { res = sglm_v2(4.0f, 0.0f);  } break;
        case '%': { res = sglm_v2(5.0f, 0.0f);  } break;
        case '&': { res = sglm_v2(6.0f, 0.0f);  } break;
        case '(': { res = sglm_v2(8.0f, 0.0f);  } break;
        case ')': { res = sglm_v2(9.0f, 0.0f);  } break;
        case '*': { res = sglm_v2(10.0f, 0.0f); } break;
        case '+': { res = sglm_v2(11.0f, 0.0f); } break;
        case '_': { res = sglm_v2(13.0f, 0.0f); } break;
        case '/': { res = sglm_v2(15.0f, 0.0f); } break;
        case '0': { res = sglm_v2(0.0f, 1.0f);  } break;
        case '1': { res = sglm_v2(1.0f, 1.0f);  } break;
        case '2': { res = sglm_v2(2.0f, 1.0f);  } break;
        case '3': { res = sglm_v2(3.0f, 1.0f);  } break;
        case '4': { res = sglm_v2(4.0f, 1.0f);  } break;
        case '5': { res = sglm_v2(5.0f, 1.0f);  } break;
        case '6': { res = sglm_v2(6.0f, 1.0f);  } break;
        case '7': { res = sglm_v2(7.0f, 1.0f);  } break;
        case '8': { res = sglm_v2(8.0f, 1.0f);  } break;
        case '9': { res = sglm_v2(9.0f, 1.0f);  } break;
        case ':': { res = sglm_v2(10.0f, 1.0f); } break;
        case '.': { res = sglm_v2(10.0f, 1.0f); } break; // TODO(Jonny): Hacky
        case ',': { res = sglm_v2(10.0f, 1.0f); } break; // TODO(Jonny): Hacky
        case ';': { res = sglm_v2(11.0f, 1.0f); } break;
        case '<': { res = sglm_v2(12.0f, 1.0f); } break;
        case '=': { res = sglm_v2(13.0f, 1.0f); } break;
        case '>': { res = sglm_v2(14.0f, 1.0f); } break;
        case '?': { res = sglm_v2(15.0f, 1.0f); } break;
        case '@': { res = sglm_v2(0.0f, 2.0f);  } break;
        case 'A': { res = sglm_v2(1.0f, 2.0f);  } break;
        case 'B': { res = sglm_v2(2.0f, 2.0f);  } break;
        case 'C': { res = sglm_v2(3.0f, 2.0f);  } break;
        case 'D': { res = sglm_v2(4.0f, 2.0f);  } break;
        case 'E': { res = sglm_v2(5.0f, 2.0f);  } break;
        case 'F': { res = sglm_v2(6.0f, 2.0f);  } break;
        case 'G': { res = sglm_v2(7.0f, 2.0f);  } break;
        case 'H': { res = sglm_v2(8.0f, 2.0f);  } break;
        case 'I': { res = sglm_v2(9.0f, 2.0f);  } break;
        case 'J': { res = sglm_v2(10.0f, 2.0f); } break;
        case 'K': { res = sglm_v2(11.0f, 2.0f); } break;
        case 'L': { res = sglm_v2(12.0f, 2.0f); } break;
        case 'M': { res = sglm_v2(13.0f, 2.0f); } break;
        case 'N': { res = sglm_v2(14.0f, 2.0f); } break;
        case 'O': { res = sglm_v2(15.0f, 2.0f); } break;
        case 'P': { res = sglm_v2(0.0f, 3.0f);  } break;
        case 'Q': { res = sglm_v2(1.0f, 3.0f);  } break;
        case 'R': { res = sglm_v2(2.0f, 3.0f);  } break;
        case 'S': { res = sglm_v2(3.0f, 3.0f);  } break;
        case 'T': { res = sglm_v2(4.0f, 3.0f);  } break;
        case 'U': { res = sglm_v2(5.0f, 3.0f);  } break;
        case 'V': { res = sglm_v2(6.0f, 3.0f);  } break;
        case 'W': { res = sglm_v2(7.0f, 3.0f);  } break;
        case 'X': { res = sglm_v2(8.0f, 3.0f);  } break;
        case 'Y': { res = sglm_v2(9.0f, 3.0f);  } break;
        case 'Z': { res = sglm_v2(10.0f, 3.0f); } break;
        case '[': { res = sglm_v2(11.0f, 3.0f); } break;
        case ']': { res = sglm_v2(13.0f, 3.0f); } break;
        case '^': { res = sglm_v2(14.0f, 3.0f); } break;
        case '-': { res = sglm_v2(15.0f, 3.0f); } break;
        case 'a': { res = sglm_v2(1.0f, 4.0f);  } break;
        case 'b': { res = sglm_v2(2.0f, 4.0f);  } break;
        case 'c': { res = sglm_v2(3.0f, 4.0f);  } break;
        case 'd': { res = sglm_v2(4.0f, 4.0f);  } break;
        case 'e': { res = sglm_v2(5.0f, 4.0f);  } break;
        case 'f': { res = sglm_v2(6.0f, 4.0f);  } break;
        case 'g': { res = sglm_v2(7.0f, 4.0f);  } break;
        case 'h': { res = sglm_v2(8.0f, 4.0f);  } break;
        case 'i': { res = sglm_v2(9.0f, 4.0f);  } break;
        case 'j': { res = sglm_v2(10.0f, 4.0f); } break;
        case 'k': { res = sglm_v2(11.0f, 4.0f); } break;
        case 'l': { res = sglm_v2(12.0f, 4.0f); } break;
        case 'm': { res = sglm_v2(13.0f, 4.0f); } break;
        case 'n': { res = sglm_v2(14.0f, 4.0f); } break;
        case 'o': { res = sglm_v2(15.0f, 4.0f); } break;
        case 'p': { res = sglm_v2(0.0f, 5.0f);  } break;
        case 'q': { res = sglm_v2(1.0f, 5.0f);  } break;
        case 'r': { res = sglm_v2(2.0f, 5.0f);  } break;
        case 's': { res = sglm_v2(3.0f, 5.0f);  } break;
        case 't': { res = sglm_v2(4.0f, 5.0f);  } break;
        case 'u': { res = sglm_v2(5.0f, 5.0f);  } break;
        case 'v': { res = sglm_v2(6.0f, 5.0f);  } break;
        case 'w': { res = sglm_v2(7.0f, 5.0f);  } break;
        case 'x': { res = sglm_v2(8.0f, 5.0f);  } break;
        case 'y': { res = sglm_v2(9.0f, 5.0f);  } break;
        case 'z': { res = sglm_v2(10.0f, 5.0f); } break;
        case '{': { res = sglm_v2(11.0f, 5.0f); } break;
        case '|': { res = sglm_v2(12.0f, 5.0f); } break;
        case '}': { res = sglm_v2(13.0f, 5.0f); } break;
        case '~': { res = sglm_v2(14.0f, 5.0f); } break;

        default: { assert(false); } break;
    }

    return (res);
}
