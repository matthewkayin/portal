#pragma once

#include "math/math.h"
#include "texture.h"
#include <SDL2/SDL.h>

struct RendererLight {
    vec3 position;
    vec3 color;
};

bool renderer_init(SDL_Window* window, ivec2 screen_size, ivec2 window_size);
void renderer_quit();
void renderer_prepare_frame();
void renderer_present_frame();

void renderer_set_clear_color(vec3 color);
void renderer_set_lights(const RendererLight* lights, int light_count);
void renderer_set_camera(vec3 position, vec3 target);

void renderer_render_light(vec3 position);
void renderer_render_quad3d(const Transform& transform, Texture texture);