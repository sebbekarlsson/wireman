#ifndef WIREMAN_ACTOR_EFFECT_H
#define WIREMAN_ACTOR_EFFECT_H
#include <cengine/actor.h>


typedef struct ACTOR_EFFECT_STRUCT
{
    actor_T base;

    enum
    {
        EFFECT_EXPLOSION
    } type;
} actor_effect_T;

actor_effect_T* init_actor_effect(float x, float y, float z, int type);

void actor_effect_tick(actor_T* self);

void actor_effect_draw(actor_T* self);
#endif
