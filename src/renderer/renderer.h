#pragma once

#include "math/math.h"

struct SDL_Window;

bool renderer_init(SDL_Window* window, ivec2 screen_size, ivec2 window_size);
void renderer_quit();
void renderer_prepare_frame();
void renderer_present_frame();