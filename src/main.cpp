#ifdef _WIN32
    #define SDL_MAIN_HANDLED
#endif

#include "core/application.h"
#include "core/input.h"
#include <cstdio>

bool init();
void update(float delta);
void render();

int main() {
    AppConfig config = (AppConfig) {
        .name = "PORTAL",
        .screen_width = 1280,
        .screen_height = 720,
        
        .resource_path = "./res/",

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
        printf("hey\n");
    }
    if (input_is_input_just_released(INPUT_FORWARD)) {
        printf("ho\n");
    }
    if (input_is_input_just_pressed(INPUT_PORTAL_LEFT)) {
        printf("hwey\n");
    }
}

void render() {

}