#include "include/inventory.h"
#include <cengine/texture.h>
#include <cengine/application.h>
#include <cengine/keyboard.h>
#include <stdlib.h>


extern texture_T* TEXTURE_ENTITY_SHEET;
extern application_T* APP;

inventory_T* init_inventory()
{
    inventory_T* inventory = calloc(1, sizeof(struct INVENTORY_STRUCT));
    
    for (int i = 0; i < INVENTORY_SIZE; i++)
        inventory->slots[i] = (inventory_slot_T){(void*)0, 0};

    inventory->draw_program = init_draw_program(APP->shader_program_default);

    inventory->index = 0;

    return inventory;
}

void inventory_tick(inventory_T* inventory)
{
    if (keyboard_check_pressed_once(GLFW_KEY_Q))
    {
        inventory->index = inventory->index >= INVENTORY_SIZE-1 ? 0 : inventory->index + 1;
    }
}

void inventory_draw(inventory_T* inventory)
{
    for (int i = 0; i < INVENTORY_SIZE; i++)
    {
        inventory_slot_T slot = inventory->slots[i];

        int tex_x = 0;
        int tex_y = 15;

        if (i == inventory->index)
            tex_x = 1;

        draw_texture(
            inventory->draw_program,
            TEXTURE_ENTITY_SHEET->id,
            8 + ((32 + 1) * i), (RES_HEIGHT-32) - 8, 3.0f,
            32,
            32,
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
        
        if (slot.item == (void*)0)
            continue;

        draw_texture(
            inventory->draw_program,
            slot.item->texture->id,
            8 + (((32 + 1) * i) + slot.item->texture->width / 2), ((RES_HEIGHT-32) - 8) + slot.item->texture->height / 2, 4.0f,
            slot.item->texture->width,
            slot.item->texture->height,
            255,
            255,
            255,
            1.0f,
            slot.item->texture->shift_x,
            slot.item->texture->shift_y,
            16,
            16,
            0,
            0
        );
    }
}
