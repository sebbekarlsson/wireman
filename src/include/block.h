#ifndef WIREMAN_BLOCK_H
#define WIREMAN_BLOCK_H
#include <cengine/texture.h>


#define BLOCK_SIZE 32
#define NR_BLOCKS 2
#define BLOCK_MAX_CHARGE 8

typedef struct BLOCK_STRUCT
{
    enum
    {
        BLOCK_AIR = 0,
        BLOCK_STONE,
        BLOCK_WIRE,
        BLOCK_LEVER,
        BLOCK_LAMP,
        BLOCK_DOOR,
        BLOCK_DYNAMITE
    } type;

    enum
    {
        BLOCK_NO_ALT,
        BLOCK_WIRE_VERTICAL,
        BLOCK_WIRE_HORIZONTAL,
        BLOCK_WIRE_BOTTOM_LEFT,
        BLOCK_WIRE_BOTTOM_RIGHT,
        BLOCK_WIRE_TOP_LEFT,
        BLOCK_WIRE_TOP_RIGHT,

        BLOCK_DOOR_VERTICAL,
        BLOCK_DOOR_HORIZONTAL
    } alt;

    unsigned int charge;


    enum
    {
        CHARGE_DIR_NONE,
        CHARGE_DIR_LEFT,
        CHARGE_DIR_RIGHT,
        CHARGE_DIR_UP,
        CHARGE_DIR_DOWN
    } charge_dir;

    unsigned int solid;
    double last_update;

    texture_T* texture;
} block_T;

block_T* init_block(int type);

void get_block_atlascoords(int type, int alt, int charge, int* x, int* y);

void block_update(block_T* block, void* chunkptr, int x, int y, int z);

void block_tick(block_T* block);

void block_set_solid(block_T* block, int type);
#endif
