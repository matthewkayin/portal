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

enum AppMouseMode {
    APP_MOUSE_MODE_VISIBLE,
    APP_MOUSE_MODE_RELATIVE
};

bool application_create(AppConfig config);
void application_run();
uint32_t application_get_fps();

AppMouseMode application_get_mouse_mode();
void application_set_mouse_mode(AppMouseMode mouse_mode);