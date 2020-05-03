#include "include/item.h"
#include "include/block.h"
#include <stdlib.h>


extern texture_T* TEXTURE_SHEET;

item_T* init_item(int type)
{
    item_T* item = calloc(1, sizeof(struct ITEM_STRUCT));
    item->type = type;
    item->texture = init_texture(TEXTURE_SHEET->id, 16, 16);

    int tex_x = 0;
    int tex_y = 0;

    switch (type)
    {
        case ITEM_WIRE: tex_x = 1; tex_y = 2; item->block = BLOCK_WIRE; break;
        case ITEM_LEVER: tex_x = 1; tex_y = 6; item->block = BLOCK_LEVER; break;
        default: /* silence */ break;
    }

    item->texture->shift_x = tex_x;
    item->texture->shift_y = tex_y;

    return item;
}
