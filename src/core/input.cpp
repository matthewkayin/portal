#include "input.h"

#include "logger.h"
#include <cstdio>

struct InputState {
    ivec2 mouse_position;
    ivec2 mouse_relative_position;

    bool current[INPUT_COUNT];
    bool previous[INPUT_COUNT];
};

static InputState input_state;

void input_init() {
    memset(&input_state, 0, sizeof(InputState));
    log_info("Input subsystem initialized.");
}

void input_update() {
    input_state.mouse_relative_position = ivec2(0, 0);

    memcpy(&input_state.previous, &input_state.current, sizeof(bool) * INPUT_COUNT);
}

void input_process_key(SDL_Keycode key, bool pressed) {
    auto it = input_key_to_input_map.find(key);
    if (it == input_key_to_input_map.end()) {
        return;
    }
    Input input = it->second;
    input_state.current[input] = pressed;
}

void input_process_mouse_button(uint8_t button, bool pressed) {
    auto it = input_mouse_button_to_input_map.find(button);
    if (it == input_mouse_button_to_input_map.end()) {
        return;
    }
    Input input = it->second;
    input_state.current[input] = pressed;
}

void input_process_mouse_motion(ivec2 mouse_position, ivec2 mouse_relative_position) {
    input_state.mouse_position = mouse_position;
    input_state.mouse_relative_position = mouse_relative_position;
}

bool input_is_action_pressed(Input input) {
    return input_state.current[input];
}

bool input_is_action_just_pressed(Input input) {
    return input_state.current[input] && !input_state.previous[input];
}

bool input_is_action_just_released(Input input) {
    return !input_state.current[input] && input_state.previous[input];
}

ivec2 input_get_mouse_position() {
    return input_state.mouse_position;
}

ivec2 input_get_mouse_relative_position() {
    return input_state.mouse_relative_position;
}