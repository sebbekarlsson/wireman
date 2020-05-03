#ifndef WIREMAN_MAIN_SCENE_H
#define WIREMAN_MAIN_SCENE_H
#include <cengine/scene.h>
#include "chunk.h"
#include "actor_player.h"


#define NR_CHUNKS 8

typedef struct MAIN_SCENE_STRUCT
{
    scene_T base;
    chunk_T* chunks[NR_CHUNKS][NR_CHUNKS];
    actor_player_T* player;
} main_scene_T;

main_scene_T* init_main_scene();

void main_scene_tick(scene_T* self);

void main_scene_pre_draw(scene_T* self);

void main_scene_post_draw(scene_T* self);

chunk_T* main_scene_get_chunk(main_scene_T* main_scene, float x, float y);

block_T* main_scene_get_block(main_scene_T* main_scene, float x, float y, float z);

void main_scene_set_block(main_scene_T* main_scene, float x, float y, float z, int type);
#endif
