#pragma once

#include "math/math.h"
#include <SDL2/SDL.h>
#include <unordered_map>

enum Input {
    INPUT_FORWARD,
    INPUT_RIGHT,
    INPUT_BACK,
    INPUT_LEFT,
    INPUT_JUMP,
    INPUT_CROUCH,
    INPUT_PORTAL_LEFT,
    INPUT_PORTAL_RIGHT,
    INPUT_ESCAPE,
    INPUT_TILDE,
    INPUT_COUNT
};

const std::unordered_map<SDL_Keycode, Input> input_key_to_input_map {
    { SDLK_w, INPUT_FORWARD },
    { SDLK_d, INPUT_RIGHT },
    { SDLK_s, INPUT_BACK },
    { SDLK_a, INPUT_LEFT },
    { SDLK_SPACE, INPUT_JUMP },
    { SDLK_LCTRL, INPUT_CROUCH },
    { SDLK_ESCAPE, INPUT_ESCAPE },
    { SDLK_BACKQUOTE, INPUT_TILDE }
};

const std::unordered_map<uint8_t, Input> input_mouse_button_to_input_map {
    { SDL_BUTTON_LEFT, INPUT_PORTAL_LEFT },
    { SDL_BUTTON_RIGHT, INPUT_PORTAL_RIGHT },
};

void input_init();
void input_update();

void input_process_key(SDL_Keycode key, bool pressed);
void input_process_mouse_button(uint8_t button, bool pressed);
void input_process_mouse_motion(ivec2 mouse_position, ivec2 mouse_relative_position);

bool input_is_action_pressed(Input input);
bool input_is_action_just_pressed(Input input);
bool input_is_action_just_released(Input input);

ivec2 input_get_mouse_position();
ivec2 input_get_mouse_relative_position();