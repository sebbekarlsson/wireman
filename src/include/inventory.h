#ifndef WIREMAN_INVENTORY_H
#define WIREMAN_INVENTORY_H
#include <cengine/draw.h>

#define INVENTORY_SIZE 4

typedef struct INVENTORY_SLUT_STRUCT
{
    int block_type;
    unsigned int quantity;
} inventory_slot_T;

typedef struct INVENTORY_STRUCT
{
    inventory_slot_T slots[INVENTORY_SIZE];
    draw_program_T* draw_program;
    draw_program_T* draw_program_text;
    unsigned int index;
} inventory_T;

inventory_T* init_inventory();

void inventory_tick(inventory_T* inventory);

void inventory_draw(inventory_T* inventory);

void inventory_add_item(inventory_T* inventory, int block_type);
#endif
