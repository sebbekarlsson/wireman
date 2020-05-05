#include "include/block.h"
#include <cengine/application.h>
#include <stdlib.h>
#include <sys/param.h>


extern texture_T* TEXTURE_SHEET;

block_T* init_block(int type)
{
    block_T* block = calloc(1, sizeof(struct BLOCK_STRUCT));
    block->type = type;
    block->alt = 0;
    block->electric = 0;
    block->charged = 0;
    block->last_update = application_get_time();
    block_set_solid(block, type);

    block->texture = init_texture(TEXTURE_SHEET->id, 32, 32);

    block_tick(block); 

    return block;
}

void get_block_atlascoords(int type, int alt, int* x, int* y)
{
    int tex_x = 0;
    int tex_y = 0;

    switch (type)
    {
        case BLOCK_WIRE: tex_x = 1; tex_y = 2; break;
        case BLOCK_LEVER: tex_x = 1; tex_y = 6; break;
        case BLOCK_LAMP: tex_x = 1; tex_y = 7; break;
        case BLOCK_DYNAMITE: tex_x = 3; tex_y = 0; break;
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

void block_tick(block_T* block)
{
    int tex_x;
    int tex_y;
    get_block_atlascoords(block->type, block->alt, &tex_x, &tex_y); 

    if (block->type == BLOCK_WIRE || block->type == BLOCK_LEVER || block->type == BLOCK_LAMP || block->type == BLOCK_DOOR)
        tex_x = 1 + (int)MIN(1, block->electric);

    if (block->type == BLOCK_DYNAMITE)
        tex_x = 3 + (int)MIN(1, block->electric);


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
