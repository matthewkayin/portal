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

struct AppState {
    bool (*on_init)();
    void (*on_switch)(void* switch_params);
    void (*update)(float delta);
    void (*render)();
};

enum AppMouseMode {
    APP_MOUSE_MODE_VISIBLE,
    APP_MOUSE_MODE_RELATIVE
};

bool application_create(AppConfig config);
bool application_register_state(int state_id, AppState app_state);
void application_set_state(int state_id, void* switch_params);
void application_run(int initial_state_id);
uint32_t application_get_fps();

AppMouseMode application_get_mouse_mode();
void application_set_mouse_mode(AppMouseMode mouse_mode);