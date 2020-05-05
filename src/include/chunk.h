#ifndef WIREMAN_CHUNK_H
#define WIREMAN_CHUNK_H
#include <cengine/draw.h>
#include "block.h"

#define CHUNK_SIZE 8

typedef struct CHUNK_STRUCT
{
    block_T* blocks[CHUNK_SIZE][CHUNK_SIZE][2];
    int x;
    int y;
    draw_program_T* draw_program;
} chunk_T;

chunk_T* init_chunk(int x, int y);

void chunk_tick(chunk_T* chunk);

void chunk_draw(chunk_T* chunk);

void chunk_tick_block(chunk_T* chunk, int x, int y, int z);

void chunk_tick_electric(chunk_T* chunk, block_T* source, int x, int y, int z);
#endif
