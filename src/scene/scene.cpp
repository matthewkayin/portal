#include "scene.h"

#include "core/logger.h"
#include "core/application.h"
#include "core/input.h"
#include "renderer/renderer.h"

struct State {
    // Player
    vec3 player_position;
    vec3 player_direction;
    float player_camera_yaw;
    float player_camera_pitch;

    // Lights
    vec3 light_position;
};

static State state;

bool scene_init() {
    // Initialize player
    state.player_position = vec3(0.0f);
    state.player_camera_yaw = deg_to_rad(-90.0f);
    state.player_camera_pitch = 0.0f;

    state.light_position = vec3(0.0f, 0.75f, -5.0f);

    return true;
}

void scene_update(float delta) {
    static const float CAMERA_PITCH_LIMIT = deg_to_rad(89.0f);
    static const float CAMERA_SPEED = 0.1f;
    static const float PLAYER_SPEED = 5.0f;

    // Player input
    ivec2 player_move_input = ivec2(0, 0);
    if (application_get_mouse_mode() == APP_MOUSE_MODE_VISIBLE) {
        if (input_is_action_just_pressed(INPUT_PORTAL_LEFT)) {
            application_set_mouse_mode(APP_MOUSE_MODE_RELATIVE);
        }
    } else {
        if (input_is_action_just_pressed(INPUT_ESCAPE)) {
            application_set_mouse_mode(APP_MOUSE_MODE_VISIBLE);
        }

        // Looking
        ivec2 mouse_motion = input_get_mouse_relative_position();
        state.player_camera_yaw += deg_to_rad(mouse_motion.x * -CAMERA_SPEED);
        state.player_camera_pitch = clampf(state.player_camera_pitch + deg_to_rad(mouse_motion.y * CAMERA_SPEED), -CAMERA_PITCH_LIMIT, CAMERA_PITCH_LIMIT);

        // Movement 
        if (input_is_action_pressed(INPUT_FORWARD)) {
            player_move_input.y -= 1;
        }
        if (input_is_action_pressed(INPUT_BACK)) {
            player_move_input.y += 1;
        }
        if (input_is_action_pressed(INPUT_LEFT)) {
            player_move_input.x -= 1;
        }
        if (input_is_action_pressed(INPUT_RIGHT)) {
            player_move_input.x += 1;
        }
    }

    // Player update
    state.player_direction = vec3(cos(state.player_camera_yaw) * cos(state.player_camera_pitch),
                                  sin(state.player_camera_pitch),
                                  sin(state.player_camera_yaw) * cos(state.player_camera_pitch));
    vec3 player_move_forward_direction = vec3(state.player_direction.x, 0.0f, state.player_direction.z).normalized();
    vec3 player_move_right_direction = vec3::cross(player_move_forward_direction, VEC3_UP).normalized();
    vec3 player_velocity = ((player_move_forward_direction * -player_move_input.y) + 
                           (player_move_right_direction * player_move_input.x)).normalized() * PLAYER_SPEED;
    state.player_position += player_velocity * delta;
}

void scene_render() {
    renderer_set_camera(state.player_position, state.player_position + state.player_direction);
    renderer_render_light(state.light_position);
}