#include "editor.h"

#include "renderer/renderer.h"
#include "math/math.h"
#include "core/application.h"
#include "core/input.h"
#include "core/logger.h"
#include "states/states.h"

bool editor_init() {
    return true;
}

void editor_on_switch(void* switch_params) {
    renderer_set_clear_color(vec3(0.8f, 0.8f, 0.8f));
    application_set_mouse_mode(APP_MOUSE_MODE_VISIBLE);
}

void editor_update(float delta) {
    if (input_is_action_just_pressed(INPUT_TILDE)) {
        application_set_state(STATE_LEVEL, nullptr);
    }
}

void editor_render() {
}