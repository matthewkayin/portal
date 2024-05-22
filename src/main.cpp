#ifdef _WIN32
    #define SDL_MAIN_HANDLED
#endif

#include "core/application.h"
#include "core/logger.h"
#include "core/input.h"
#include <cstdio>

bool init();
void update(float delta);
void render();

int main() {
    AppConfig config = (AppConfig) {
        .name = "PORTAL",
        .screen_size = ivec2(1280, 720),
        .window_size = ivec2(1280, 720),
        
        .resource_path = "../res/",

        .init = &init,
        .update = &update,
        .render = &render
    };
    if (!application_create(config)) {
        return -1;
    }

    application_run();

    return 0;
}

bool init() {
    return true;
}

void update(float delta) {
    if (input_is_input_just_pressed(INPUT_FORWARD)) {
        log_info("hey\n");
    }
    if (input_is_input_just_released(INPUT_FORWARD)) {
        log_info("ho\n");
    }
    if (input_is_input_just_pressed(INPUT_PORTAL_LEFT)) {
        log_info("hwey\n");
    }
}

void render() {

}