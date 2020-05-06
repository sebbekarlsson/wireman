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

    for (int i = 0; i < CHUNK_SIZE; i++)
        for (int j = 0; j < CHUNK_SIZE; j++)
            for (int k = 0; k < 2; k++)
                chunk->blocks[i][j][k] = init_block(BLOCK_AIR);

    chunk->draw_program = init_draw_program(APP->shader_program_default);
    draw_program_genbuffers(chunk->draw_program);

    return chunk;
}

void chunk_tick(chunk_T* chunk)
{
    for (int x = 0; x < CHUNK_SIZE; x++)
    {
        for (int y = 0; y < CHUNK_SIZE; y++)
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

    actor_T* actor_camera = (actor_T*) scene->camera;

    glBindVertexArray(scene->VAO);

    /*draw_line(
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
    );*/

    for (int x = 0; x < CHUNK_SIZE; x++)
    {
        for (int y = 0; y < CHUNK_SIZE; y++)
        {
            for (int z = 0; z < 2; z++)
            {
                block_T* block = chunk->blocks[x][y][z];

                if (block->type == BLOCK_AIR)
                    continue;

                int block_x = chunk->x + (x * BLOCK_SIZE);
                int block_y = chunk->y + (y * BLOCK_SIZE);

                if (
                    block_x + BLOCK_SIZE < actor_camera->x ||
                    block_x > actor_camera->x + RES_WIDTH ||

                    block_y > actor_camera->y + RES_HEIGHT ||
                    block_y + BLOCK_SIZE < actor_camera->y
                )
                    continue;

                float color_c = (2 - z);

                float r = 255 / color_c;
                float g = 255 / color_c;
                float b = 255 / color_c;

                draw_texture(
                    chunk->draw_program,
                    TEXTURE_SHEET->id,
                    chunk->x + (x*BLOCK_SIZE), chunk->y + (y*BLOCK_SIZE), z,
                    BLOCK_SIZE,
                    BLOCK_SIZE,
                    r,
                    g,
                    b,
                    1.0f,
                    block->texture->shift_x,
                    block->texture->shift_y,
                    16,
                    16,
                    0,
                    0
                );
            }
        }
    }
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

    if (current_type == BLOCK_DOOR || current_type == BLOCK_DYNAMITE)
        return 1;

    if (prev_alt == BLOCK_WIRE_HORIZONTAL || prev_alt == BLOCK_WIRE_VERTICAL)
        if (current_type == BLOCK_LAMP || current_type == BLOCK_DOOR)
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



void chunk_tick_block(chunk_T* chunk, int x, int y, int z)
{
    block_T* block = chunk->blocks[x][y][z];

    double current_time = application_get_time();
    
    if ((current_time - block->last_update) < 0.5f)
        return;

    block_T* block_left = chunk->blocks[MAX(0, x-1)][y][z];
    block_T* block_right = chunk->blocks[MIN(CHUNK_SIZE-1, x+1)][y][z];
    block_T* block_up = chunk->blocks[x][MAX(0, y-1)][z];
    block_T* block_down = chunk->blocks[x][MIN(CHUNK_SIZE-1, y+1)][z];

    block_tick(block);
    block_update(block, chunk, x, y, z);

    block->last_update = application_get_time(APP);
}
