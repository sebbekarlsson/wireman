#include "include/block.h"
#include "include/chunk.h"
#include "include/main_scene.h"
#include "include/actor_effect.h"
#include <cengine/application.h>
#include <stdlib.h>
#include <sys/param.h>


extern texture_T* TEXTURE_SHEET;
extern application_T* APP;

block_T* init_block(int type)
{
    block_T* block = calloc(1, sizeof(struct BLOCK_STRUCT));
    block->type = type;
    block->alt = 0;
    block->charge = 0;
    block->last_update = application_get_time();
    block_set_solid(block, type);

    block->texture = init_texture(TEXTURE_SHEET->id, 32, 32);

    block_tick(block); 

    return block;
}

void get_block_atlascoords(int type, int alt, int charge, int* x, int* y)
{
    int tex_x = 0;
    int tex_y = 0;

    switch (type)
    {
        case BLOCK_WIRE: tex_x = 1 + MIN(1, charge > 0); tex_y = 2; break;
        case BLOCK_LEVER: tex_x = 1 + MIN(1, charge > 0); tex_y = 6; break;
        case BLOCK_LAMP: tex_x = 1 + MIN(1, charge > 0); tex_y = 7; break;
        case BLOCK_DOOR: tex_x = 1 + MIN(1, charge > 0); tex_y = 9; break;
        case BLOCK_DYNAMITE: tex_x = 3 + MIN(1, charge > 0); tex_y = 0; break;
        default: /* silence */ break;
    }

    switch (alt)
    {
        case BLOCK_WIRE_VERTICAL: tex_y = 0; break;
        case BLOCK_WIRE_HORIZONTAL: tex_y = 1; break;
        case BLOCK_WIRE_BOTTOM_LEFT: tex_y = 2; break;
        case BLOCK_WIRE_BOTTOM_RIGHT: tex_y = 3; break;
        case BLOCK_WIRE_TOP_LEFT: tex_y = 4; break;
        case BLOCK_WIRE_TOP_RIGHT: tex_y = 5; break;
        
        case BLOCK_DOOR_VERTICAL: tex_y = 9; break;
        case BLOCK_DOOR_HORIZONTAL: tex_y = 8; break;
        default: /* silence */ break;
    }

    *x = tex_x;
    *y = tex_y;
}

void block_update(block_T* block, void* chunkptr, int x, int y, int z)
{
    chunk_T* chunk = (chunk_T*) chunkptr;
    scene_T* scene = application_get_current_scene(APP);
    main_scene_T* main_scene = (main_scene_T*) scene;
    

    block_T* block_left = chunk->blocks[MAX(0, x-1)][y][z];
    block_T* block_right = chunk->blocks[MIN(CHUNK_SIZE-1, x+1)][y][z];
    block_T* block_up = chunk->blocks[x][MAX(0, y-1)][z];
    block_T* block_down = chunk->blocks[x][MIN(CHUNK_SIZE-1, y+1)][z];

    if (block->type == BLOCK_WIRE)
    {
        block->alt = BLOCK_WIRE_VERTICAL;

        // update alternatives
        if (block_left->type == BLOCK_WIRE || block_right->type == BLOCK_WIRE)
            block->alt = BLOCK_WIRE_HORIZONTAL;
        if (block_up->type == BLOCK_WIRE)
            block->alt = BLOCK_WIRE_VERTICAL;
        if (block_up->type == BLOCK_WIRE && block_right->type == BLOCK_WIRE)
            block->alt = BLOCK_WIRE_BOTTOM_LEFT;
        if (block_up->type == BLOCK_WIRE && block_left->type == BLOCK_WIRE)
            block->alt = BLOCK_WIRE_BOTTOM_RIGHT;
        if (block_down->type == BLOCK_WIRE && block_right->type == BLOCK_WIRE)
            block->alt = BLOCK_WIRE_TOP_LEFT;
        if (block_down->type == BLOCK_WIRE && block_left->type == BLOCK_WIRE)
            block->alt = BLOCK_WIRE_TOP_RIGHT;
    }

    if (block->type != BLOCK_AIR && block->type != BLOCK_LEVER)
    {
        block->charge = 0;

        if (block_right->charge > block_left->charge && block_right->charge > block_down->charge && block_right->charge > block_up->charge)
            block->charge = MAX(0, (block_right->charge - 1));

        if (block_left->charge > block_right->charge && block_left->charge > block_down->charge && block_left->charge > block_up->charge )
            block->charge = MAX(0, (block_left->charge - 1));

        if (block_up->charge > block_down->charge && block_up->charge > block_left->charge && block_up->charge > block_right->charge)
            block->charge = MAX(0, (block_up->charge - 1));

        if (block_down->charge > block_up->charge && block_down->charge > block_left->charge && block_down->charge > block_right->charge)
            block->charge = MAX(0, (block_down->charge - 1));
    }

    if (block->type == BLOCK_DOOR)
        block->solid = (block->charge <= 0);

    if (block->type == BLOCK_DYNAMITE)
    {
        if (block->charge)
        {
            scene_add_actor(scene, (actor_T*) init_actor_effect(chunk->x + (x*BLOCK_SIZE), chunk->y + (y*BLOCK_SIZE), 3.0f, EFFECT_EXPLOSION));
            block->type = BLOCK_AIR;
            block->charge = 0;
            block->alt = 0;
        }
    }
}

void block_tick(block_T* block)
{
    int tex_x;
    int tex_y;
    get_block_atlascoords(block->type, block->alt, block->charge, &tex_x, &tex_y); 

    block->texture->shift_x = tex_x;
    block->texture->shift_y = tex_y;
}

void block_set_solid(block_T* block, int type)
{
    switch (block->type)
    {
        case BLOCK_STONE: case BLOCK_DOOR: block->solid = 1; break;
        default: block->solid = 0; break;
    }
}
