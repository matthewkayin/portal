#pragma once

#include "math/math.h"
#include <SDL2/SDL.h>

bool renderer_init(SDL_Window* window, ivec2 screen_size, ivec2 window_size);
void renderer_quit();
void renderer_prepare_frame();
void renderer_present_frame();

void renderer_set_clear_color(vec3 color);
void renderer_set_camera(vec3 position, vec3 target);
void renderer_render_light(vec3 position);