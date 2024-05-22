#pragma once

#include <cstdint>

struct AppConfig {
    const char* name;
    int screen_width;
    int screen_height;

    const char* resource_path;

    bool (*init)();
    void (*update)(float delta);
    void (*render)();

};

bool application_create(AppConfig config);
void application_run();
uint32_t application_get_fps();