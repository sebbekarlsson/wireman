#include "include/chunk.h"
#include <cengine/draw.h>
#include <cengine/application.h>
#include <stdlib.h>
#include <sys/param.h>

extern texture_T* TEXTURE_SHEET;
extern application_T* APP;


chunk_T* init_chunk(int x, int y)
{
    chunk_T* chunk = calloc(1, sizeof(struct CHUNK_STRUCT));
    chunk->x = x;
    chunk->y = y;

    for (int i = 0; i < 16; i++)
        for (int j = 0; j < 16; j++)
            for (int k = 0; k < 2; k++)
                chunk->blocks[i][j][k] = init_block(BLOCK_AIR);

    chunk->draw_program = init_draw_program(APP->shader_program_default);

    return chunk;
}

void chunk_tick(chunk_T* chunk)
{
    for (int x = 0; x < 16; x++)
    {
        for (int y = 0; y < 16; y++)
        {
            for (int z = 0; z < 2; z++)
            {
                block_T* block = chunk->blocks[x][y][z];

                if (block->type == BLOCK_AIR)
                    continue;

                chunk_tick_block(chunk, x, y, z);
            }
        }
    }
}

void chunk_draw(chunk_T* chunk)
{
    scene_T* scene = application_get_current_scene(APP);

    glBindVertexArray(scene->VAO);

    draw_line(
        scene->draw_program_color,
        chunk->x, chunk->y, 0,
        chunk->x + CHUNK_SIZE * BLOCK_SIZE, chunk->y, 0,
        255, 0, 0, 1.0f 
    );

    draw_line(
        scene->draw_program_color,
        chunk->x, chunk->y + CHUNK_SIZE * BLOCK_SIZE, 0,
        chunk->x, chunk->y, 0,
        0, 255, 0, 1.0f 
    );

    for (int x = 0; x < 16; x++)
    {
        for (int y = 0; y < 16; y++)
        {
            for (int z = 0; z < 2; z++)
            {
                block_T* block = chunk->blocks[x][y][z];

                if (block->type == BLOCK_AIR)
                    continue;

                int tex_x = 0;
                int tex_y = 0;
            
                if (block->type == BLOCK_STONE)
                {
                    tex_x = 0;
                    tex_y = 0;
                }
                else
                if (block->type == BLOCK_WIRE)
                {
                    tex_x = 1 + (int)MIN(1, block->electric);

                    switch (block->alt)
                    {
                        case BLOCK_WIRE_VERTICAL: tex_y = 0; break;
                        case BLOCK_WIRE_HORIZONTAL: tex_y = 1; break;
                        case BLOCK_WIRE_BOTTOM_LEFT: tex_y = 2; break;
                        case BLOCK_WIRE_BOTTOM_RIGHT: tex_y = 3; break;
                        case BLOCK_WIRE_TOP_LEFT: tex_y = 4; break;
                        case BLOCK_WIRE_TOP_RIGHT: tex_y = 5; break;
                        default: /* silence */ break;
                    }
                }
                else
                if (block->type == BLOCK_LEVER)
                {
                    tex_y = 6;
                    tex_x = 1 + (int)MIN(1, block->electric);
                }
                else
                if (block->type == BLOCK_LAMP)
                {
                    tex_y = 7;
                    tex_x = 1 + (int)MIN(1, block->electric);
                }

                glBindVertexArray(scene->VAO);

                draw_texture(
                    chunk->draw_program,
                    TEXTURE_SHEET->id,
                    chunk->x + (x*BLOCK_SIZE), chunk->y + (y*BLOCK_SIZE), z,
                    BLOCK_SIZE,
                    BLOCK_SIZE,
                    255,
                    255,
                    255,
                    1.0f,
                    tex_x,
                    tex_y,
                    16,
                    16,
                    0,
                    0
                );
            }
        }
    }
}

static unsigned int block_is_source(block_T* block)
{
    return block->type == BLOCK_LEVER;
}

double to_positive_angle(double angle)
{
   angle = fmod(angle, 360);
   if (angle < 0) angle += 360;
   return angle;
}

static unsigned int electric_can_move(int prev_alt, int current_type, int current_alt, int prev_angle, int current_angle)
{
    if (current_type == BLOCK_AIR)
        return 0;

    if (prev_alt == BLOCK_WIRE_HORIZONTAL || prev_alt == BLOCK_WIRE_VERTICAL)
        if (current_type == BLOCK_LAMP)
            return 1;

    if (prev_alt == 0)
        return 1;
    
    if (
        prev_alt == BLOCK_WIRE_VERTICAL &&
        (
            current_alt == BLOCK_WIRE_VERTICAL ||
            current_alt == BLOCK_WIRE_TOP_LEFT ||
            current_alt == BLOCK_WIRE_TOP_RIGHT ||
            current_alt == BLOCK_WIRE_BOTTOM_LEFT ||
            current_alt == BLOCK_WIRE_BOTTOM_RIGHT
        )
     )
    {
        if (prev_angle == 270 && (current_alt == BLOCK_WIRE_TOP_RIGHT || current_alt == BLOCK_WIRE_TOP_LEFT))
            return 0;

        return prev_angle == current_angle;
    }

    if (
        prev_alt == BLOCK_WIRE_HORIZONTAL &&
        (
            current_alt == BLOCK_WIRE_HORIZONTAL ||
            current_alt == BLOCK_WIRE_TOP_LEFT ||
            current_alt == BLOCK_WIRE_TOP_RIGHT ||
            current_alt == BLOCK_WIRE_BOTTOM_LEFT ||
            current_alt == BLOCK_WIRE_BOTTOM_RIGHT
        )
     )
    {
        if (prev_angle == 180 && (current_alt == BLOCK_WIRE_TOP_RIGHT || current_alt == BLOCK_WIRE_BOTTOM_RIGHT))
            return 0;
        return prev_angle == current_angle;
    }

     if (
         prev_alt == BLOCK_WIRE_TOP_LEFT &&
         (
            current_alt == BLOCK_WIRE_HORIZONTAL ||
            current_alt == BLOCK_WIRE_BOTTOM_RIGHT  ||
            current_alt == BLOCK_WIRE_BOTTOM_LEFT  ||
            current_alt == BLOCK_WIRE_VERTICAL
         )
     )
     {
         if (prev_angle == 180)
             return current_angle == 270;

         return current_angle == 0;
     }

    if (
         prev_alt == BLOCK_WIRE_TOP_RIGHT &&
         (
            current_alt == BLOCK_WIRE_HORIZONTAL ||
            current_alt == BLOCK_WIRE_BOTTOM_LEFT ||
            current_alt == BLOCK_WIRE_BOTTOM_RIGHT ||
            current_alt == BLOCK_WIRE_VERTICAL
         )
     )
     {
         if (prev_angle == 0)
             return current_angle == 270;

         return current_angle == 180;
     }

    if (
         prev_alt == BLOCK_WIRE_BOTTOM_LEFT &&
         (
            current_alt == BLOCK_WIRE_HORIZONTAL ||
            current_alt == BLOCK_WIRE_BOTTOM_LEFT ||
            current_alt == BLOCK_WIRE_VERTICAL ||
            current_alt == BLOCK_WIRE_TOP_RIGHT
         )
     )
    {
        if (prev_angle == 270)
            return current_angle == 0;

        return current_angle == 90;
    }

    if (
         prev_alt == BLOCK_WIRE_BOTTOM_RIGHT &&
         (
            current_alt == BLOCK_WIRE_HORIZONTAL ||
            current_alt == BLOCK_WIRE_BOTTOM_LEFT ||
            current_alt == BLOCK_WIRE_VERTICAL ||
            current_alt == BLOCK_WIRE_TOP_LEFT
         )
     )
    {
        if (prev_angle == 270)
            return current_angle == 180;

         return current_angle == 90;
    }

    return 0;
}

void chunk_tick_electric(chunk_T* chunk, block_T* source, int x, int y, int z)
{
    int length = 8;

    int px = x;
    int py = y;
    int prev_alt = 0;
    int prev_v = 0;

    const int angles[4] = {90, 0, 270, 180};

    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            int v = angles[j];
            int xm = (int)(float)(px + (cos(glm_rad(v)) * 1.0f));
            int ym = (int)(float)(py - (sin(glm_rad(v)) * 1.0f));

            block_T* new_block = chunk->blocks[xm][ym][z];

            if (electric_can_move(prev_alt, new_block->type, new_block->alt, prev_v, v))
            {
                prev_alt = new_block->alt;

                new_block->charged = source->electric;

                px = xm;
                py = ym;
                prev_v = v;

                break;
            }
        }
    }
}

static void chunk_tick_block_wire(
    chunk_T* chunk,
    block_T* block,
    block_T* block_left,
    block_T* block_right,
    block_T* block_up,
    block_T* block_down
)
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

    block->electric = block->charged;

    // update electricity
    /*if (
        ((block_left->alt == BLOCK_WIRE_HORIZONTAL || block_left->alt == BLOCK_WIRE_BOTTOM_LEFT || block_left->alt == BLOCK_WIRE_TOP_LEFT || block_is_source(block_left)) && block_left->electric) &&
        (block->alt == BLOCK_WIRE_HORIZONTAL || block->alt == BLOCK_WIRE_TOP_RIGHT || block->alt == BLOCK_WIRE_BOTTOM_RIGHT)
    )
        block->electric = 1;

    if (
        ((block_right->alt == BLOCK_WIRE_HORIZONTAL || block_right->alt == BLOCK_WIRE_BOTTOM_LEFT || block_right->alt == BLOCK_WIRE_TOP_RIGHT || block_is_source(block_right)) && block_right->electric) &&
        (block->alt == BLOCK_WIRE_HORIZONTAL || block->alt == BLOCK_WIRE_TOP_LEFT || block->alt == BLOCK_WIRE_BOTTOM_LEFT)
    )
        block->electric = 1;

    if (
        ((block_down->alt == BLOCK_WIRE_VERTICAL || block_down->alt == BLOCK_WIRE_BOTTOM_LEFT || block_down->alt == BLOCK_WIRE_BOTTOM_RIGHT || block_is_source(block_down)) && block_down->electric) &&
        (block->alt == BLOCK_WIRE_VERTICAL || block->alt == BLOCK_WIRE_TOP_LEFT || block->alt == BLOCK_WIRE_TOP_RIGHT)
    )
        block->electric = 1;

    if (
        ((block_up->alt == BLOCK_WIRE_VERTICAL || block_up->alt == BLOCK_WIRE_TOP_LEFT || block_up->alt == BLOCK_WIRE_TOP_RIGHT || block_is_source(block_up)) && block_up->electric) &&
        (block->alt == BLOCK_WIRE_VERTICAL || block->alt == BLOCK_WIRE_BOTTOM_LEFT || block->alt == BLOCK_WIRE_BOTTOM_RIGHT) 
    )
        block->electric = 1;

    if (
        (block_right->alt == BLOCK_WIRE_BOTTOM_RIGHT && block_right->electric) &&
        (block->alt == BLOCK_WIRE_TOP_LEFT || block->alt == BLOCK_WIRE_HORIZONTAL)        
    )
        block->electric = 1;*/
}

void chunk_tick_block(chunk_T* chunk, int x, int y, int z)
{
    block_T* block = chunk->blocks[x][y][z];

    double current_time = application_get_time();
    
    if ((current_time - block->last_update) < 0.5f)
        return;

    block_T* block_left = chunk->blocks[MAX(0, x-1)][y][z];
    block_T* block_right = chunk->blocks[MIN(CHUNK_SIZE, x+1)][y][z];
    block_T* block_up = chunk->blocks[x][MAX(0, y-1)][z];
    block_T* block_down = chunk->blocks[x][MIN(CHUNK_SIZE, y+1)][z];

    switch (block->type)
    {
        case BLOCK_WIRE: chunk_tick_block_wire(chunk, block, block_left, block_right, block_up, block_down); break;
        case BLOCK_LEVER: chunk_tick_electric(chunk, block, x, y, z); break;
        default: /* silence */ break;
    }

    block->last_update = application_get_time(APP);
}
