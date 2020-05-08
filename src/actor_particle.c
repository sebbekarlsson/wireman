#include "include/actor_particle.h"
#include <cengine/application.h>
#include <cengine/physics.h>


extern application_T* APP;


actor_particle_T* init_actor_particle(float x, float y, float z)
{
    actor_particle_T* particle = calloc(1, sizeof(struct ACTOR_PARTICLE_STRUCT));
    particle->duration = rand() % (100 + 1 - 50) + 50;
    particle->min_size = 2;
    particle->max_size = 8;
    particle->min_force = 150.0f;
    particle->max_force = 300.0f;
    particle->r = 255;
    particle->g = 255;
    particle->b = 255;
    particle->a = 1.0f;
    particle->pushed = 0;
    particle->direction = rand() % (360 + 1 - 0) + 0;
    particle->size = rand() % (particle->max_size + 1 - particle->min_size) + particle->min_size;
    particle->force = rand() % (particle->max_force + 1 - particle->min_force) + particle->min_force;

    actor_T* actor = actor_constructor((actor_T*) particle, x, y, z);
    actor->tick = actor_particle_tick;
    actor->draw = actor_particle_draw;
    actor->friction = 1.0f;

    return particle;
}

void actor_particle_tick(actor_T* self)
{
    scene_T* scene = application_get_current_scene(APP);
    actor_particle_T* particle = (actor_particle_T*) self;

    if (particle->duration <= 0)
    {
        scene_remove_actor(scene, self);
        return;
    }

    if (!particle->pushed)
    {
        physics_vec2_push(&self->dx, &self->dy, particle->direction, particle->force * APP->delta_time);
        particle->pushed = 1;
    }

    physics_tick_actor(self);
    particle->duration -= 1;
}

void actor_particle_draw(actor_T* self)
{
    actor_particle_T* particle = (actor_particle_T*) self;
    scene_T* scene = application_get_current_scene(APP);

    //glBindVertexArray(scene->VAO);

    draw_quad(
        scene->draw_program_color,
        particle->size, particle->size,
        self->x, self->y, self->z,
        particle->r, particle->g, particle->b, particle->a
    );
}
