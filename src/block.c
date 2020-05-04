#include "include/block.h"
#include <cengine/application.h>
#include <stdlib.h>


block_T* init_block(int type)
{
    block_T* block = calloc(1, sizeof(struct BLOCK_STRUCT));
    block->type = type;
    block->alt = 0;
    block->electric = 0;
    block->charged = 0;
    block->last_update = application_get_time();

    return block;
}
