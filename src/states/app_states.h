#pragma once

#include "states.h"
#include "level/level.h"
#include "editor/editor.h"
#include "core/application.h"
#include <unordered_map>

static const std::unordered_map<State, AppState> app_states = {
    { STATE_LEVEL, (AppState) {
        .on_init = &level_init,
        .on_switch = &level_on_switch,
        .update = &level_update,
        .render = &level_render
    }},
    { STATE_EDITOR, (AppState) {
        .on_init = &editor_init,
        .on_switch = &editor_on_switch,
        .update = &editor_update,
        .render = &editor_render
    }}
};