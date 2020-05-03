#ifndef WIREMAN_ACTOR_PLAYER_H
#define WIREMAN_ACTOR_PLAYER_H
#include <cengine/actor.h>
#include "inventory.h"


typedef struct ACTOR_PLAYER_STRUCT 
{
    actor_T base;
    sprite_T* sprite_down;
    sprite_T* sprite_side;
    sprite_T* sprite_up;

    inventory_T* inventory;
} actor_player_T;

actor_player_T* init_actor_player(float x, float y, float z);

void actor_player_tick(actor_T* self);

void actor_player_draw(actor_T* self);
#endif
