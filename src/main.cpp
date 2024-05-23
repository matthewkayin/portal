#ifdef _WIN32
    #define SDL_MAIN_HANDLED
#endif

#include "core/application.h"
#include "core/logger.h"
#include "core/input.h"
#include "scene/scene.h"
#include <cstdio>

int main() {
    AppConfig config = (AppConfig) {
        .name = "PORTAL",
        .screen_size = ivec2(1280, 720),
        .window_size = ivec2(1280, 720),
        
        .resource_path = "../res/",

        .init = &scene_init,
        .update = &scene_update,
        .render = &scene_render
    };
    if (!application_create(config)) {
        return -1;
    }

    application_run();

    return 0;
}