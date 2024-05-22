#pragma once

#include "math/vector2.h"
#include <cstdint>

struct AppConfig {
    const char* name;
    ivec2 screen_size;
    ivec2 window_size;

    const char* resource_path;

    bool (*init)();
    void (*update)(float delta);
    void (*render)();

};

bool application_create(AppConfig config);
void application_run();
uint32_t application_get_fps();