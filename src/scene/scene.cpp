#include "scene.h"

#include "core/logger.h"
#include "core/application.h"
#include "core/input.h"
#include "renderer/renderer.h"

struct Player {
    vec3 position;
    vec3 direction;
    float camera_yaw;
    float camera_pitch;
};

struct State {
    Player player;
    vec3 light_position;
};

static State state;

bool scene_init() {
    state.player.position = vec3(0.0f);
    state.player.camera_yaw = deg_to_rad(-90.0f);
    state.player.camera_pitch = 0.0f;

    state.light_position = vec3(0.0f, 0.75f, -5.0f);

    return true;
}

void scene_update(float delta) {
    static const float CAMERA_PITCH_LIMIT = deg_to_rad(89.0f);
    static const float CAMERA_SPEED = 0.1f;

    // Player input
    if (application_get_mouse_mode() == APP_MOUSE_MODE_VISIBLE) {
        if (input_is_action_just_pressed(INPUT_PORTAL_LEFT)) {
            application_set_mouse_mode(APP_MOUSE_MODE_RELATIVE);
        }
    } else {
        if (input_is_action_just_pressed(INPUT_ESCAPE)) {
            application_set_mouse_mode(APP_MOUSE_MODE_VISIBLE);
        }

        ivec2 mouse_motion = input_get_mouse_relative_position();
        state.player.camera_yaw += deg_to_rad(mouse_motion.x * -CAMERA_SPEED);
        state.player.camera_pitch = clampf(state.player.camera_pitch + deg_to_rad(mouse_motion.y * CAMERA_SPEED), -CAMERA_PITCH_LIMIT, CAMERA_PITCH_LIMIT);
    }

    state.player.direction = vec3(cos(state.player.camera_yaw) * cos(state.player.camera_pitch),
                                  sin(state.player.camera_pitch),
                                  sin(state.player.camera_yaw) * cos(state.player.camera_pitch));
}

void scene_render() {
    renderer_set_camera(state.player.position, state.player.direction);
    renderer_render_light(state.light_position);
}