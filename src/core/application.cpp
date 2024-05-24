#include "application.h"

#include "resource.h"
#include "logger.h"
#include "input.h"
#include "renderer/renderer.h"
#include <SDL2/SDL.h>
#include <cstdio>
#include <unordered_map>

static const uint64_t FRAME_TIME = (uint64_t)(1000.0 / 60.0);
static const int APP_STATE_NONE = -1;

struct Application {
    SDL_Window* window;
    AppMouseMode mouse_mode;

    uint32_t fps;

    int state_id;
    std::unordered_map<int, AppState> states;
};

static bool initialized = false;
static Application app;
std::string resource_base_path;

bool application_create(AppConfig config) {
    if (initialized) {
        log_error("application_create() called more than once.");
        return false;
    }

    logger_init();

    // Get info out of config
    resource_base_path = std::string(config.resource_path);

    // Initialize other fields
    app.state_id = APP_STATE_NONE;

    // Init SDL
    SDL_GL_LoadLibrary(NULL);
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        log_error("SDL failed to initialize: %s", SDL_GetError());
    }

    // Create window
    app.window = SDL_CreateWindow(config.name, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, config.window_size.x, config.window_size.y, SDL_WINDOW_OPENGL);
    if (app.window == NULL) {
        log_error("Error creating window: %s", SDL_GetError());
        return false;
    }

    // Initialize subsystems
    input_init();
    if (!renderer_init(app.window, config.screen_size, config.window_size)) { return false; }

    log_info("%s initialized.", config.name);

    return true;
}

bool application_register_state(int state_id, AppState app_state) {
    // Check that the state pointers are filled out
    if (!app_state.on_init || !app_state.on_switch || !app_state.update || !app_state.render) {
        log_error("application_register_state(): Function pointers are not filled out for app state with id %i", state_id);
        return false;
    }

    // Check to make sure the state hasn't already been registered
    if (app.states.find(state_id) != app.states.end()) {
        log_error("application_register_state(): State with id %i has already been registered.", state_id);
        return false;
    }

    // Initialize the state
    if (!app_state.on_init()) {
        log_error("application_register_state(): State with id %i did not initialize successfully.", state_id);
        return false;
    }

    // Put the state in the states list
    app.states[state_id] = app_state;

    return true;
}

void application_set_state(int state_id, void* switch_params) {
    if (app.states.find(state_id) == app.states.end()) {
        log_error("Cannot switch state. The state id %i does not exist.", state_id);
        return;
    }

    app.state_id = state_id;
    app.states[app.state_id].on_switch(switch_params);
}

void application_run(int initial_state_id) {
    if (app.states.find(initial_state_id) == app.states.end()) {
        log_error("Cannot run application. The initial state id %i does not exist.", initial_state_id);
        return;
    }

    app.state_id = initial_state_id;
    bool is_running = true;
    uint64_t last_time = SDL_GetTicks();
    uint64_t last_second = last_time;
    uint32_t frames = 0;
    float delta = 0.0f;

    while (is_running) {
        // Timekeep
        uint64_t current_time = SDL_GetTicks();
        while (current_time - last_time < FRAME_TIME) {
            current_time = SDL_GetTicks();
        }

        delta = (float)(current_time - last_time) / 1000.0f;
        last_time = current_time;

        if (current_time - last_second >= 1000) {
            app.fps = frames;
            frames = 0;
            last_second += 1000;
        }

        frames++;

        // Input
        input_update();
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    is_running = false;
                    break;
                case SDL_KEYDOWN:
                    input_process_key(event.key.keysym.sym, true);
                    break;
                case SDL_KEYUP:
                    input_process_key(event.key.keysym.sym, false);
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    input_process_mouse_button(event.button.button, true);
                    break;
                case SDL_MOUSEBUTTONUP:
                    input_process_mouse_button(event.button.button, false);
                    break;
                case SDL_MOUSEMOTION:
                    input_process_mouse_motion(ivec2(event.motion.x, event.motion.y), ivec2(event.motion.xrel, event.motion.yrel));
                    break;
            }
        }

        // Update
        app.states[app.state_id].update(delta);
        
        // Render
        renderer_prepare_frame();
        app.states[app.state_id].render();
        renderer_present_frame();
    }

    // Quit subsystems
    renderer_quit();

    SDL_DestroyWindow(app.window);

    SDL_Quit();

    log_info("Application quit gracefully.");
    logger_quit();
}

uint32_t application_get_fps() {
    return app.fps;
}

AppMouseMode application_get_mouse_mode() {
    return app.mouse_mode;
}

void application_set_mouse_mode(AppMouseMode mouse_mode) {
    if (app.mouse_mode == mouse_mode) {
        return;
    }

    app.mouse_mode = mouse_mode;
    if (app.mouse_mode == APP_MOUSE_MODE_VISIBLE) {
        SDL_SetRelativeMouseMode(SDL_FALSE);
    } else {
        SDL_SetRelativeMouseMode(SDL_TRUE);
    }
}