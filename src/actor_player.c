#include "include/actor_player.h"
#include "include/main_scene.h"
#include <cengine/keyboard.h>
#include <cengine/physics.h>
#include <cengine/application.h>
#include <GLFW/glfw3.h>


extern texture_T* TEXTURE_ENTITY_SHEET;
extern application_T* APP;


actor_player_T* init_actor_player(float x, float y, float z)
{
    actor_player_T* actor_player = calloc(1, sizeof(struct ACTOR_PLAYER_STRUCT));
    actor_T* actor = actor_constructor((actor_T*) actor_player, x, y, z);

    actor->tick = actor_player_tick;
    actor->draw = actor_player_draw;

    actor->friction = 5.0f;

    actor_player->sprite_down = init_sprite();
    actor_player->sprite_down->delay = 0.2f;

    actor_player->inventory = init_inventory();
    actor_player->inventory->slots[0] = (inventory_slot_T){BLOCK_WIRE, 16};
    actor_player->inventory->slots[1] = (inventory_slot_T){BLOCK_LEVER, 4};
    actor_player->inventory->slots[2] = (inventory_slot_T){BLOCK_LAMP, 2};
    actor_player->inventory->slots[3] = (inventory_slot_T){BLOCK_DYNAMITE, 2};

    actor_player->health = 100.0f;
    
    texture_T* texture_down = init_texture(TEXTURE_ENTITY_SHEET->id, TEXTURE_ENTITY_SHEET->width, TEXTURE_ENTITY_SHEET->height);
    texture_down->atlas_width = 16;
    texture_down->atlas_height = 16;
    texture_down->shift_x = 0;
    texture_down->shift_y = 0;
    texture_down->width = 32;
    texture_down->height = 32;

    sprite_add_texture(actor_player->sprite_down, texture_down);

    actor_player->sprite_side = init_sprite();
    actor_player->sprite_side->delay = 0.2f;
    
    texture_T* texture_side = init_texture(TEXTURE_ENTITY_SHEET->id, TEXTURE_ENTITY_SHEET->width, TEXTURE_ENTITY_SHEET->height);
    texture_side->atlas_width = 16;
    texture_side->atlas_height = 16;
    texture_side->shift_x = 1;
    texture_side->shift_y = 0;
    texture_side->width = 32;
    texture_side->height = 32;

    sprite_add_texture(actor_player->sprite_side, texture_side);

    actor_player->sprite_up = init_sprite();
    actor_player->sprite_up->delay = 0.2f;
    
    texture_T* texture_up = init_texture(TEXTURE_ENTITY_SHEET->id, TEXTURE_ENTITY_SHEET->width, TEXTURE_ENTITY_SHEET->height);
    texture_up->atlas_width = 16;
    texture_up->atlas_height = 16;
    texture_up->shift_x = 2;
    texture_up->shift_y = 0;
    texture_up->width = 32;
    texture_up->height = 32;
    
    sprite_add_texture(actor_player->sprite_up, texture_up);

    actor->sprite = actor_player->sprite_down;

    return actor_player;
}

static void move(actor_T* self, float xa, float ya)
{
    if (xa != 0 && ya != 0)
    {
        move(self, xa, 0);
        move(self, 0, ya);
        return;
    }

    float w = 32;
    float h = 32;

    scene_T* scene = application_get_current_scene(APP);
    main_scene_T* main_scene = (main_scene_T*) scene;
    

    float next_x = (self->x + 16) + xa + (xa > 0 ? 16 : xa < 0 ? -16 : 0);
    float next_y = (self->y + 16) + ya + (ya > 0 ? 16 : ya < 0 ? -16 : 0);

    chunk_T* chunk = main_scene_get_chunk(main_scene, next_x, next_y); 

    for (int x = 0; x < CHUNK_SIZE; x++)
    {
        for (int y = 0; y < CHUNK_SIZE; y++)
        {
            float bx = chunk->x + (x*32);
            float by = chunk->y + (y*32);

            block_T* block = chunk->blocks[x][y][1];

            if (!block->solid)
                continue;

            if (self->x+30+xa > bx && self->x+xa < bx + w)
            {
                if ((self->y+30) > by && self->y < by+h)
                {
                    self->dx = 0;
                    return;
                }

            }

            if (self->y+30+ya > by && self->y+ya < by + h)
            {
                if ((self->x+30) > bx && self->x < bx+w)
                {
                    self->dy = 0;
                    return;
                }
            }
        } 
    } 

    self->x += xa;
    self->y += ya;
}

void actor_player_tick(actor_T* self)
{
    scene_T* scene = application_get_current_scene(APP);
    main_scene_T* main_scene = (main_scene_T*) scene;

    actor_player_T* player = (actor_player_T*) self;

    float speed = 10.0f;

    physics_to_zero(&self->dx, self->friction);
    physics_to_zero(&self->dy, self->friction);

    if (keyboard_check_pressed(GLFW_KEY_LEFT))
    {
        physics_vec2_push(&self->dx, &self->dy, 180, speed * APP->delta_time);
        self->sprite = player->sprite_side;
    }

    if (keyboard_check_pressed(GLFW_KEY_RIGHT))
    {
        physics_vec2_push(&self->dx, &self->dy, 0, speed * APP->delta_time);
        self->sprite = player->sprite_side;
    }

    if (keyboard_check_pressed(GLFW_KEY_UP))
    {
        physics_vec2_push(&self->dx, &self->dy, 90, speed * APP->delta_time);
        self->sprite = player->sprite_up;
    }

    if (keyboard_check_pressed(GLFW_KEY_DOWN))
    {
        physics_vec2_push(&self->dx, &self->dy, 270, speed * APP->delta_time);
        self->sprite = player->sprite_down;
    }

    if (keyboard_check_pressed_once(GLFW_KEY_SPACE))
    {
        inventory_slot_T slot = player->inventory->slots[player->inventory->index];
        
        if (slot.block_type != BLOCK_AIR)
        {
            main_scene_set_block(main_scene, self->x + 32/2, self->y + 32/2, 1, slot.block_type);
            player->inventory->slots[player->inventory->index] = (inventory_slot_T){slot.block_type, slot.quantity-1};
        }
    }

    if (keyboard_check_pressed_once(GLFW_KEY_X))
    {
        block_T* block = main_scene_get_block(main_scene, self->x +32/2, self->y + 32/2, 1);
        
        if (block->type != BLOCK_AIR)
        {
            inventory_add_item(player->inventory, block->type);
            main_scene_set_block(main_scene, self->x + 32/2, self->y + 32/2, 1, BLOCK_AIR);
        }
    }

    if (keyboard_check_pressed_once(GLFW_KEY_E))
    {
        block_T* block = main_scene_get_block(main_scene, self->x +32/2, self->y + 32/2, 1);

        if (block->type == BLOCK_LEVER)
            block->electric = !block->electric;
    }

    if (self->dx > 0)
    {
        self->sprite->flip_x = 0;
        self->sprite->animated = 1;
    }
    else
    if (self->dx < 0)
    {
        self->sprite->flip_x = 1;
        self->sprite->animated = 1;
    }
    else
    {
        self->sprite->animated = 0;
    }

    move(self, self->dx, self->dy);
}

void actor_player_draw(actor_T* self)
{
}
