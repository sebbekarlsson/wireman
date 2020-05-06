#include "include/actor_effect.h"
#include <cengine/application.h>


extern texture_T* TEXTURE_EFFECTS_SHEET;
extern application_T* APP;

actor_effect_T* init_actor_effect(float x, float y, float z, int type)
{
    actor_effect_T* actor_effect = calloc(1, sizeof(struct ACTOR_EFFECT_STRUCT));
    actor_effect->type = type;
    actor_T* actor = actor_constructor((actor_T*)actor_effect, x, y, z);
    actor->sprite = init_sprite();
    actor->sprite->delay = 0.3f;

    actor->tick = actor_effect_tick;

    if (type == EFFECT_EXPLOSION)
    {
        for (int i = 0; i < 4; i++)
        {
            texture_T* texture = init_texture(TEXTURE_EFFECTS_SHEET->id, 32, 32);
            texture->shift_x = 15;
            texture->shift_y = i;
            texture->atlas_width = 16;
            texture->atlas_height = 16;
            sprite_add_texture(actor->sprite, texture);
        }
    }

    return actor_effect;
}

void actor_effect_tick(actor_T* self)
{
    scene_T* scene = application_get_current_scene(APP);

    if (self->sprite->index >= self->sprite->textures->size-1)
        scene_remove_actor(scene, self);
}

void actor_effect_draw(actor_T* self)
{
}
