#include "include/main_scene.h"
#include "include/actor_player.h"
#include "include/inventory.h"
#include <cengine/window.h>
#include <sys/param.h>


extern texture_T* TEXTURE_ENTITY_SHEET;

main_scene_T* init_main_scene()
{
    main_scene_T* main_scene = calloc(1, sizeof(struct MAIN_SCENE_STRUCT));
    scene_T* scene = scene_constructor((scene_T*) main_scene);

    scene->tick = main_scene_tick;
    scene->pre_draw = main_scene_pre_draw;
    scene->draw = main_scene_post_draw;

    for (int x = 0; x < NR_CHUNKS; x++)
        for (int y = 0; y < NR_CHUNKS; y++)
            main_scene->chunks[x][y] = init_chunk(x*32*CHUNK_SIZE, y*32*CHUNK_SIZE);


    for (int x = 0; x < CHUNK_SIZE*2; x++)
    {
        for (int y = 0; y < CHUNK_SIZE*2; y++)
        {
            main_scene_set_block(main_scene, x*BLOCK_SIZE, y*BLOCK_SIZE, 0, BLOCK_STONE);
            main_scene_set_block(main_scene, x*BLOCK_SIZE, y*BLOCK_SIZE, 1, BLOCK_AIR);

            if ((y == 0 && x < CHUNK_SIZE) || (y == CHUNK_SIZE-1 && x < CHUNK_SIZE) || (x == 0 && y < CHUNK_SIZE) || (x == CHUNK_SIZE-1 && y < CHUNK_SIZE))
                main_scene_set_block(main_scene, x*BLOCK_SIZE, y*BLOCK_SIZE, 1, BLOCK_STONE);
        }
    }


    main_scene_set_block(main_scene, (CHUNK_SIZE-1)*BLOCK_SIZE, 4*BLOCK_SIZE, 1, BLOCK_DOOR)->alt = BLOCK_DOOR_VERTICAL;
    
    main_scene->player = init_actor_player(BLOCK_SIZE*4, BLOCK_SIZE*4, 2.0f);
    scene_add_actor(scene, (actor_T*) main_scene->player);

    return main_scene;
}


void main_scene_tick(scene_T* scene)
{
    main_scene_T* main_scene = (main_scene_T*) scene;
    
    actor_T* actor_camera = (actor_T*) scene->camera;

    inventory_tick(main_scene->player->inventory);

    for (int x = 0; x < NR_CHUNKS; x++)
    {
        for (int y = 0; y < NR_CHUNKS; y++)
        {
            chunk_T* chunk = main_scene->chunks[x][y];

            if (
                chunk->x + (CHUNK_SIZE*32) < actor_camera->x ||
                chunk->x > actor_camera->x + RES_WIDTH ||

                chunk->y > actor_camera->y + RES_HEIGHT ||
                chunk->y + (CHUNK_SIZE*32) < actor_camera->y
            )
                continue;

            chunk_tick(chunk);
        }
    }

    ((actor_T*)scene->camera)->x = (((actor_T*)main_scene->player)->x - RES_WIDTH/2) + 32/2;
    ((actor_T*)scene->camera)->y = (((actor_T*)main_scene->player)->y - RES_HEIGHT/2) + 32/2;
}

void main_scene_pre_draw(scene_T* scene)
{
    main_scene_T* main_scene = (main_scene_T*) scene;

    actor_T* actor_camera = (actor_T*) scene->camera;

    camera_bind(scene->camera);

    for (int x = 0; x < NR_CHUNKS; x++)
    {
        for (int y = 0; y < NR_CHUNKS; y++)
        {
            chunk_T* chunk = main_scene->chunks[x][y];

            if (
                chunk->x + (CHUNK_SIZE*32) < actor_camera->x ||
                chunk->x > actor_camera->x + RES_WIDTH ||

                chunk->y > actor_camera->y + RES_HEIGHT ||
                chunk->y + (CHUNK_SIZE*32) < actor_camera->y
            )
                continue;

            chunk_draw(chunk);
        }
    }

    camera_unbind(scene->camera);
}

void main_scene_post_draw(scene_T* scene)
{
    main_scene_T* main_scene = (main_scene_T*) scene;

    inventory_draw(main_scene->player->inventory);

    for (int i = 0; i < (int)(main_scene->player->health * 0.05f); i++)
    {
        int texture_width = 32;
        int texture_height = 32;

        glBindVertexArray(scene->VAO);

        draw_options_T draw_options = DRAW_OPTIONS_INIT;
        draw_options.texture = TEXTURE_ENTITY_SHEET->id;
        draw_options.x = 8 + ((texture_width+1)*4) + (((32 + 1) * i));
        draw_options.y = ((RES_HEIGHT-32) - 8);
        draw_options.z = 4.0f;
        draw_options.width = texture_width;
        draw_options.height = texture_height;
        draw_options.r = 255;
        draw_options.g = 255;
        draw_options.b = 255;
        draw_options.shift_x = 0;
        draw_options.shift_y = 14;
        draw_options.atlas_width = 16;
        draw_options.atlas_height = 16;

        draw_texture(
            scene->draw_program,
            draw_options
        );
    }
}

static int mod(int x, int N)
{
    return (x % N + N) %N;
}

chunk_T* main_scene_get_chunk(main_scene_T* main_scene, float x, float y)
{
    int chunk_x = x / (32*CHUNK_SIZE);
    int chunk_y = y / (32*CHUNK_SIZE);

    return main_scene->chunks[MIN(NR_CHUNKS-1, MAX(0, chunk_x))][MIN(NR_CHUNKS-1, MAX(0, chunk_y))];
}

block_T* main_scene_get_block(main_scene_T* main_scene, float x, float y, float z)
{
    chunk_T* chunk = main_scene_get_chunk(main_scene, x, y);

    int bx = mod(x/32, CHUNK_SIZE);
    int by = mod(y/32, CHUNK_SIZE);

    return chunk->blocks[MIN(CHUNK_SIZE-1, bx)][MIN(CHUNK_SIZE-1, by)][MIN(2, (int)z)];
}

block_T* main_scene_set_block(main_scene_T* main_scene, float x, float y, float z, int type)
{
    chunk_T* chunk = main_scene_get_chunk(main_scene, x, y);

    int bx = mod(x/32, CHUNK_SIZE);
    int by = mod(y/32, CHUNK_SIZE);

    block_T* block = chunk->blocks[MIN(CHUNK_SIZE-1, bx)][MIN(CHUNK_SIZE-1, by)][MIN(2, (int)z)];
    block->type = type;
    block->charge = 0;
    block->alt = 0;
    block_set_solid(block, block->type);

    return block;
}
