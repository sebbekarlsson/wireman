#ifndef WIREMAN_ACTOR_PARTICLE_H
#define WIREMAN_ACTOR_PARTICLE_H
#include <cengine/actor.h>

typedef struct ACTOR_PARTICLE_STRUCT
{
    actor_T base;
    unsigned int duration;
    unsigned int min_size;
    unsigned int max_size;
    unsigned int size;
    unsigned int min_force;
    unsigned int max_force;
    unsigned int force;
    float r;
    float g;
    float b;
    float a;
    float direction;
    unsigned int pushed;
} actor_particle_T;

actor_particle_T* init_actor_particle(float x, float y, float z);

void actor_particle_tick(actor_T* self);

void actor_particle_draw(actor_T* self);
#endif
