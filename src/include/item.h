#ifndef WIREMAN_ITEM_H
#define WIREMAN_ITEM_H
#include <cengine/texture.h>


typedef struct ITEM_STRUCT
{
    enum
    {
        ITEM_WIRE,
        ITEM_LEVER,
        ITEM_LAMP
    } type;

    int block;
    
    texture_T* texture;
} item_T;

item_T* init_item(int type);
#endif
