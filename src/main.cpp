#ifdef _WIN32
    #define SDL_MAIN_HANDLED
#endif

#include "core/application.h"
#include "core/logger.h"
#include "core/input.h"
#include "states/app_states.h"
#include <cstdio>

int main() {
    AppConfig config = (AppConfig) {
        .name = "PORTAL",
        .screen_size = ivec2(1280, 720),
        .window_size = ivec2(1280, 720),
        
        .resource_path = "../res/",
    };
    if (!application_create(config)) {
        return -1;
    }

    for (int state_id = 0; state_id < STATE_COUNT; state_id++) {
        application_register_state(state_id, app_states.at((State)state_id));
    }

    application_run(STATE_LEVEL);

    return 0;
}