#ifndef WIREMAN_BLOCK_H
#define WIREMAN_BLOCK_H

#define BLOCK_SIZE 32
#define NR_BLOCKS 2

typedef struct BLOCK_STRUCT
{
    enum
    {
        BLOCK_AIR = 0,
        BLOCK_STONE,
        BLOCK_WIRE,
        BLOCK_LEVER
    } type;

    enum
    {
        BLOCK_NO_ALT,
        BLOCK_WIRE_VERTICAL,
        BLOCK_WIRE_HORIZONTAL,
        BLOCK_WIRE_BOTTOM_LEFT,
        BLOCK_WIRE_BOTTOM_RIGHT,
        BLOCK_WIRE_TOP_LEFT,
        BLOCK_WIRE_TOP_RIGHT
    } alt;

    unsigned int electric;
    double last_update;
} block_T;

block_T* init_block(int type);
#endif
