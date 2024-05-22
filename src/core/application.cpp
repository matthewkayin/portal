#include "application.h"

#include "resource.h"
#include "input.h"
#include "renderer/renderer.h"
#include <SDL2/SDL.h>
#include <cstdio>

static const uint64_t FRAME_TIME = (uint64_t)(1000.0 / 60.0);

struct Application {
    SDL_Window* window;
    ivec2 screen_size;
    ivec2 window_size;

    uint32_t fps;

    bool (*init)();
    void (*update)(float delta);
    void (*render)();
};

static bool initialized = false;
static Application app;
std::string resource_base_path;

bool application_create(AppConfig config) {
    if (initialized) {
        printf("application_create() called more than once.\n");
        return false;
    }

    // Get info out of config
    app.init = config.init;
    app.update = config.update;
    app.render = config.render;
    app.screen_size = config.screen_size;
    app.window_size = config.window_size;
    resource_base_path = std::string(config.resource_path);

    // Check to make sure config was valid
    if (!app.init || !app.update || !app.render) {
        printf("Application function pointers not assigned!\n");
        return false;
    }

    // Init SDL
    SDL_GL_LoadLibrary(NULL);
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL failed to initialize: %s", SDL_GetError());
    }

    // Create window
    app.window = SDL_CreateWindow(config.name, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, app.window_size.x, app.window_size.y, SDL_WINDOW_OPENGL);
    if (app.window == NULL) {
        printf("Error creating window: %s", SDL_GetError());
        return false;
    }

    // Initialize subsystems
    input_init();
    if (!renderer_init(app.window, app.screen_size, app.window_size)) { return false; }

    if (!app.init()) {
        printf("Application failed to initialize.\n");
        return false;
    }

    printf("%s initialized.\n", config.name);

    return true;
}

void application_run() {
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
        app.update(delta);
        
        // Render
        renderer_prepare_frame();
        app.render();
        renderer_present_frame();
    }

    // Quit subsystems
    renderer_quit();

    SDL_DestroyWindow(app.window);

    SDL_Quit();

    printf("Application quit gracefully.\n");
}

uint32_t application_get_fps() {
    return app.fps;
}