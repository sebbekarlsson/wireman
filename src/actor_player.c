#include "include/actor_player.h"
#include "include/main_scene.h"
#include "include/item.h"
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
    actor_player->inventory->slots[0] = (inventory_slot_T){init_item(ITEM_WIRE), 1};
    actor_player->inventory->slots[1] = (inventory_slot_T){init_item(ITEM_LEVER), 1};
    
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

void actor_player_tick(actor_T* self)
{
    scene_T* scene = application_get_current_scene(APP);
    main_scene_T* main_scene = (main_scene_T*) scene;

    actor_player_T* player = (actor_player_T*) self;

    float speed = 10.0f;

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

    if (keyboard_check_pressed(GLFW_KEY_SPACE))
    {
        inventory_slot_T slot = player->inventory->slots[player->inventory->index];
        
        if (slot.item != (void*)0)
            main_scene_set_block(main_scene, self->x + 32/2, self->y + 32/2, 1, slot.item->block);
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

    physics_tick_actor(self);
}

void actor_player_draw(actor_T* self)
{

    // Just drawing a cross for demonstrational purposes

    draw_line(
        self->draw_program,
        self->x, self->y + self->hitbox_height/2, self->z,
        self->x + self->hitbox_width, self->y + self->hitbox_height/2, self->z,
        255, 0, 0, 1.0f 
    );
    draw_line(
        self->draw_program,
        self->x + self->hitbox_width/2, self->y, self->z,
        self->x + self->hitbox_width/2, self->y + self->hitbox_height, self->z,
        255, 0, 0, 1.0f 
    );
}
