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

        draw_options_T draw_options0 = DRAW_OPTIONS_INIT;
        draw_options0.texture = TEXTURE_ENTITY_SHEET->id;
        draw_options0.x = 8 + ((32 + 1) * i);
        draw_options0.y = (RES_HEIGHT-32) - 8;
        draw_options0.z = 3.0f;
        draw_options0.width = 32;
        draw_options0.height = 32;
        draw_options0.r = 255;
        draw_options0.g = 255;
        draw_options0.b = 255;
        draw_options0.shift_x = tex_x;
        draw_options0.shift_y = tex_y;
        draw_options0.atlas_width = 16;
        draw_options0.atlas_height = 16;

        draw_texture(
            inventory->draw_program,
            draw_options0
        );
        
        if (slot.block_type == BLOCK_AIR)
            continue;

        int item_tex_x;
        int item_tex_y;
        get_block_atlascoords(slot.block_type, BLOCK_NO_ALT, 0, &item_tex_x, &item_tex_y);

        int texture_width = 16;
        int texture_height = 16;

        draw_options_T draw_options = DRAW_OPTIONS_INIT;
        draw_options.texture = TEXTURE_SHEET->id;
        draw_options.x = 8 + (((32 + 1) * i) + texture_width / 2);
        draw_options.y = ((RES_HEIGHT-32) - 8) + texture_height / 2;
        draw_options.z = 4.0f;
        draw_options.width = texture_width;
        draw_options.height = texture_height;
        draw_options.r = 255;
        draw_options.g = 255;
        draw_options.b = 255;
        draw_options.shift_x = item_tex_x;
        draw_options.shift_y = item_tex_y;
        draw_options.atlas_width = 16;
        draw_options.atlas_height = 16;

        draw_texture(
            inventory->draw_program,
            draw_options
        );

        const char* nr_items_template = "%d";
        char* nr_items = calloc(256, sizeof(char));
        sprintf(nr_items, nr_items_template, slot.quantity);

        draw_options_T draw_options_text = DRAW_OPTIONS_INIT;
        draw_options_text.text = nr_items;
        draw_options_text.fontpath = "/usr/local/share/fonts/AGoblinAppears-o2aV.ttf";
        draw_options_text.x = 8 + (((32 + 1) * i) + texture_width / 2);
        draw_options_text.y = ((RES_HEIGHT-32) - 8) + texture_height / 2;
        draw_options_text.z = 5.0f;
        draw_options_text.font_size = 8;
        draw_options.r = 255;
        draw_options.g = 255;
        draw_options.b = 255;

        draw_text(
            inventory->draw_program_text,
            draw_options_text
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
