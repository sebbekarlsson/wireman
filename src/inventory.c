#include "include/inventory.h"
#include "include/block.h"
#include <cengine/texture.h>
#include <cengine/application.h>
#include <cengine/keyboard.h>
#include <stdlib.h>


extern texture_T* TEXTURE_ENTITY_SHEET;
extern texture_T* TEXTURE_SHEET;
extern application_T* APP;

inventory_T* init_inventory()
{
    inventory_T* inventory = calloc(1, sizeof(struct INVENTORY_STRUCT));
    
    for (int i = 0; i < INVENTORY_SIZE; i++)
        inventory->slots[i] = (inventory_slot_T){BLOCK_AIR, 0};

    inventory->draw_program = init_draw_program(APP->shader_program_default);
    draw_program_genbuffers(inventory->draw_program);

    inventory->draw_program_text = init_draw_program(APP->shader_program_text);
    draw_program_genbuffers(inventory->draw_program_text);

    inventory->index = 0;

    return inventory;
}

void inventory_tick(inventory_T* inventory)
{
    for (int i = 0; i < INVENTORY_SIZE; i++)
    {
        inventory_slot_T slot = inventory->slots[i];

        if (slot.quantity <= 0)
            inventory->slots[i] = (inventory_slot_T){BLOCK_AIR, 0};
    }

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
        
        if (slot.block_type == BLOCK_AIR)
            continue;

        int item_tex_x;
        int item_tex_y;
        get_block_atlascoords(slot.block_type, BLOCK_NO_ALT, 0, &item_tex_x, &item_tex_y);

        int texture_width = 16;
        int texture_height = 16;

        draw_texture(
            inventory->draw_program,
            TEXTURE_SHEET->id,
            8 + (((32 + 1) * i) + texture_width / 2), ((RES_HEIGHT-32) - 8) + texture_height / 2, 4.0f,
            texture_width,
            texture_height,
            255,
            255,
            255,
            1.0f,
            item_tex_x,
            item_tex_y,
            16,
            16,
            0,
            0
        );

        const char* nr_items_template = "%d";
        char* nr_items = calloc(256, sizeof(char));
        sprintf(nr_items, nr_items_template, slot.quantity);
        
        draw_text(
            inventory->draw_program_text,
            nr_items,
            "/usr/local/share/fonts/AGoblinAppears-o2aV.ttf",
            8 + (((32 + 1) * i) + texture_width / 2), ((RES_HEIGHT-32) - 8) + texture_height / 2, 5.0f,
            8,
            255, 255, 255
        );

        free(nr_items);
    }
}


void inventory_add_item(inventory_T* inventory, int block_type)
{
    for (int i = 0; i < INVENTORY_SIZE; i++)
    {
        inventory_slot_T slot = inventory->slots[i];

        if (slot.block_type == block_type)
        {
            inventory->slots[i] = (inventory_slot_T){slot.block_type, slot.quantity+1};
            return;
        }
    }

    for (int i = 0; i < INVENTORY_SIZE; i++)
    {
        inventory_slot_T slot = inventory->slots[i];

        if (slot.block_type == BLOCK_AIR)
        {
            inventory->slots[i] = (inventory_slot_T){block_type, 1};
            return;
        }
    }
}
