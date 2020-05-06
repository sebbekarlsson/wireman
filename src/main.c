#include <cengine/main.h>
#include <cengine/application.h>
#include <cengine/texture.h>
#include "include/main_scene.h"


extern application_T* APP;

texture_T* TEXTURE_SHEET;
texture_T* TEXTURE_ENTITY_SHEET;
texture_T* TEXTURE_EFFECTS_SHEET;


int main(int argc, char* argv[])
{
    APP = init_application(640, 480, 2, "App");

    TEXTURE_SHEET = texture_get("res/img/sheet.png");
    TEXTURE_ENTITY_SHEET = texture_get("res/img/entity_sheet.png");
    TEXTURE_EFFECTS_SHEET = texture_get("res/img/effects_sheet.png");

    application_add_scene(APP, (scene_T*) init_main_scene());

    return cengine_main(argc, argv);
}
