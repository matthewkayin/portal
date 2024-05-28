#include "editor.h"

#include "renderer/renderer.h"
#include "math/math.h"
#include "core/application.h"
#include "core/input.h"
#include "core/logger.h"
#include "states/states.h"
#include "renderer/texture.h"
#include <vector>

struct Wall {
    Transform transform;
    bool portalable;
};

struct EditorState {
    Texture texture_portalwall;
    Texture texture_noportalwall;

    std::vector<Wall> walls;
    std::vector<RendererLight> lights;

    vec3 camera_position;
    float camera_yaw;
    float camera_pitch;
    float camera_distance;
    vec3 camera_target;
};

static EditorState state;

bool editor_init() {
    state.texture_portalwall = texture_acquire_solidcolor(0.78f, 0.78f, 0.78f, 1.0f);
    state.texture_noportalwall = texture_acquire_solidcolor(0.45f, 0.47f, 0.47f, 1.0f);
    state.texture_noportalwall = texture_acquire("texture/tile/diorama_tile1_05.png");

    state.walls.push_back((Wall) {
        .transform = (Transform) {
            .origin = vec3(0.0f, 0.0f, 0.0f),
            .rotation = quat(),
            .scale = vec3(1.0f)
        },
        .portalable = true
    });
    state.walls.push_back((Wall) {
        .transform = (Transform) {
            .origin = vec3(1.0f, 0.0f, 1.0f),
            .rotation = quat::from_axis_angle(VEC3_UP, deg_to_rad(90.0f), true),
            .scale = vec3(1.0f)
        },
        .portalable = true
    });
    state.walls.push_back((Wall) {
        .transform = (Transform) {
            .origin = vec3(-1.0f, 0.0f, 1.0f),
            .rotation = quat::from_axis_angle(VEC3_UP, deg_to_rad(-90.0f), true),
            .scale = vec3(1.0f)
        },
        .portalable = true
    });
    state.walls.push_back((Wall) {
        .transform = (Transform) {
            .origin = vec3(0.0f, 0.0f, 2.0f),
            .rotation = quat::from_axis_angle(VEC3_UP, deg_to_rad(180.0f), true),
            .scale = vec3(1.0f)
        },
        .portalable = true
    });
    state.walls.push_back((Wall) {
        .transform = (Transform) {
            .origin = vec3(0.0f, 1.0f, 1.0f),
            .rotation = quat::from_axis_angle(VEC3_RIGHT, deg_to_rad(90.0f), true),
            .scale = vec3(1.0f)
        },
        .portalable = true
    });
    state.walls.push_back((Wall) {
        .transform = (Transform) {
            .origin = vec3(0.0f, -1.0f, 1.0f),
            .rotation = quat::from_axis_angle(VEC3_RIGHT, deg_to_rad(-90.0f), true),
            .scale = vec3(1.0f)
        },
        .portalable = true
    });

    state.lights.push_back((RendererLight) {
        .position = vec3(0.0f, 1.0f, 0.0f),
        .color = vec3(10.0f)
    });

    state.camera_position = vec3(0.0f, -3.0f, 3.0f);
    state.camera_yaw = deg_to_rad(-90.0f);
    state.camera_pitch = deg_to_rad(45.0f);
    state.camera_distance = 3.0f;
    state.camera_target = vec3(0.0f);

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

    if (input_is_action_pressed(INPUT_PORTAL_RIGHT)) {
        ivec2 mouse_rel = input_get_mouse_relative_position();
        state.camera_yaw += mouse_rel.x * delta;
        state.camera_pitch = clampf(state.camera_pitch + (mouse_rel.y * delta), deg_to_rad(-89.0f), deg_to_rad(89.0f));
    }
    state.camera_distance = clampf(state.camera_distance - input_get_mouse_wheel_motion(), 3.0f, 25.0f);
}

void editor_render() {
    renderer_set_lights(&state.lights[0], state.lights.size());
    state.camera_position = vec3(sin(state.camera_yaw) * cos(state.camera_pitch), sin(state.camera_pitch), cos(state.camera_yaw) * cos(state.camera_pitch)) * state.camera_distance;
    renderer_set_camera(state.camera_position, state.camera_target);
    for (Wall& wall : state.walls) {
        renderer_render_quad3d(wall.transform, state.texture_noportalwall);
    }
}