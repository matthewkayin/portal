#include "renderer.h"

#include "core/logger.h"
#include "shader.h"
#include <glad/glad.h>
#include <cstdio>

struct RendererState {
    SDL_Window* window; // Pointer to the window, but it "belongs" in application
    SDL_GLContext context;

    ivec2 screen_size;
    ivec2 window_size;

    uint32_t quad_vao;
    uint32_t glyph_vao;
    uint32_t cube_vao;

    uint32_t screen_framebuffer;
    uint32_t screen_texture;
    uint32_t screen_intermediate_framebuffer;
    uint32_t screen_intermediate_texture;

    Shader screen_shader;
    Shader text_shader;
    Shader model_shader;
    Shader geometry_shader;
    Shader light_shader;
};

static RendererState state;

uint32_t primitive_quad_create();
uint32_t primitive_glyph_create();
uint32_t primitive_cube_create();

bool renderer_init(SDL_Window* window, ivec2 screen_size, ivec2 window_size) {
    state.window = window;
    state.screen_size = screen_size;
    state.window_size = window_size;

    // Set GL version
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // Create GL context
    state.context = SDL_GL_CreateContext(state.window);
    if (state.context == NULL) {
        log_error("Error creating GL context: %s", SDL_GetError());
        return false;
    }

    // Setup GLAD
    gladLoadGLLoader(SDL_GL_GetProcAddress);
    if (glGenVertexArrays == NULL) {
        log_error("Error loading OpenGL.");
        return false;
    }

    // Setup primitives
    state.quad_vao = primitive_quad_create();
    state.glyph_vao = primitive_glyph_create();
    state.cube_vao = primitive_cube_create();

    // Setup framebuffer
    glGenFramebuffers(1, &state.screen_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, state.screen_framebuffer);

    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &state.screen_texture);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, state.screen_texture);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB, screen_size.x, screen_size.y, GL_TRUE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, state.screen_texture, 0);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

    uint32_t rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, screen_size.x, screen_size.y);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        log_error("Screen framebuffer not complete!");
        return false;
    }

    glGenFramebuffers(1, &state.screen_intermediate_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, state.screen_intermediate_framebuffer);
    glGenTextures(1, &state.screen_intermediate_texture);
    glBindTexture(GL_TEXTURE_2D, state.screen_intermediate_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screen_size.x, screen_size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, state.screen_intermediate_texture, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        log_error("Screen intermediate framebuffer not complete!");
        return false;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // End setting up framebuffer

    // Load shaders
    if (!shader_load(&state.screen_shader, "shader/screen.vert.glsl", "shader/screen.frag.glsl")) {
        return false;
    }
    shader_use(state.screen_shader);
    shader_set_uniform_uint(state.screen_shader, "screen_texture", 0);

    if (!shader_load(&state.text_shader, "shader/text.vert.glsl", "shader/text.frag.glsl")) {
        return false;
    }
    shader_use(state.text_shader);
    shader_set_uniform_vec2(state.text_shader, "screen_size", vec2((float)screen_size.x, (float)screen_size.y));
    shader_set_uniform_uint(state.text_shader, "atlas_texture", 0);

    if (!shader_load(&state.model_shader, "shader/model.vert.glsl", "shader/model.frag.glsl")) {
        return false;
    }
    mat4 projection = mat4::perspective(deg_to_rad(45.0f), (float)screen_size.x / (float)screen_size.y, 0.1f, 100.0f);
    shader_use(state.model_shader);
    shader_set_uniform_mat4(state.model_shader, "projection", &projection);
    shader_set_uniform_int(state.model_shader, "material_albedo", 0);
    shader_set_uniform_int(state.model_shader, "material_metallic_roughness", 1);
    shader_set_uniform_int(state.model_shader, "material_normal", 2);
    shader_set_uniform_int(state.model_shader, "material_emissive", 3);
    shader_set_uniform_int(state.model_shader, "material_occlusion", 4);

    if (!shader_load(&state.geometry_shader, "shader/geometry.vert.glsl", "shader/geometry.frag.glsl")) {
        return false;
    }
    shader_use(state.geometry_shader);
    shader_set_uniform_mat4(state.geometry_shader, "projection", &projection);
    shader_set_uniform_int(state.geometry_shader, "material_albedo", 0);

    if (!shader_load(&state.light_shader, "shader/light.vert.glsl", "shader/light.frag.glsl")) {
        return false;
    }
    shader_use(state.light_shader);
    shader_set_uniform_mat4(state.light_shader, "projection", &projection);

    log_info("Renderer subsystem initialized.");
    return true;
}

void renderer_quit() {
    SDL_GL_DeleteContext(state.context);
}

uint32_t primitive_quad_create() {
    float quad_vertices[] = {
        // positions   // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
        1.0f, -1.0f,  1.0f, 0.0f,
        
		-1.0f,  1.0f,  0.0f, 1.0f,
        1.0f, -1.0f,  1.0f, 0.0f,
        1.0f,  1.0f,  1.0f, 1.0f
    };
    uint32_t quad_vao;
	uint32_t quad_vbo;

	glGenVertexArrays(1, &quad_vao);
	glGenBuffers(1, &quad_vbo);
	glBindVertexArray(quad_vao);
	glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), &quad_vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	glBindVertexArray(0);

    return quad_vao;
}

uint32_t primitive_glyph_create() {
    float glyph_vertices[] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        0.0f, 1.0f,

        0.0f, 1.0f,
        1.0f, 0.0f,
        1.0f, 1.0f
    };
    uint32_t glyph_vao;
    uint32_t glyph_vbo;

    glGenVertexArrays(1, &glyph_vao);
    glGenBuffers(1, &glyph_vbo);
    glBindVertexArray(glyph_vao);
    glBindBuffer(GL_ARRAY_BUFFER, glyph_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glyph_vertices), &glyph_vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

	glBindVertexArray(0);

    return glyph_vao;
}

uint32_t primitive_cube_create() {
	float cube_vertices[] = {
		// back face
		-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
		 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
		 1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
		 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
		-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
		-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
		// front face
		-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
		 1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
		 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
		 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
		-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
		-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
		// left face
		-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
		-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
		-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
		-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
		-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
		-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
		// right face
		 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
		 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
		 1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
		 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
		 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
		 1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
		 // bottom face
		 -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
		  1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
		  1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
		  1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
		 -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
		 -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
		 // top face
		 -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
		  1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
		  1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
		  1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
		 -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
		 -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
	};

    uint32_t cube_vao;
	uint32_t cube_vbo;
    
	glGenVertexArrays(1, &cube_vao);
	glGenBuffers(1, &cube_vbo);
	glBindVertexArray(cube_vao);
	glBindBuffer(GL_ARRAY_BUFFER, cube_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

	glBindVertexArray(0);

    return cube_vao;
}

void renderer_prepare_frame() {
    glBindFramebuffer(GL_FRAMEBUFFER, state.screen_framebuffer);
    glViewport(0, 0, state.screen_size.x, state.screen_size.y);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ZERO);
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void renderer_present_frame() {
    // Blit multisample buffer to intermediate buffer
    glBindFramebuffer(GL_READ_FRAMEBUFFER, state.screen_framebuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, state.screen_intermediate_framebuffer);
    glBlitFramebuffer(0, 0, state.screen_size.x, state.screen_size.y, 0, 0, state.screen_size.x, state.screen_size.y, GL_COLOR_BUFFER_BIT, GL_NEAREST);

    // Render framebuffer to screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, state.window_size.x, state.window_size.y);
    glBlendFunc(GL_ONE, GL_ZERO);
    glDisable(GL_DEPTH_TEST);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    shader_use(state.screen_shader);
    glBindVertexArray(state.quad_vao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, state.screen_intermediate_texture);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    SDL_GL_SwapWindow(state.window);
}

void renderer_set_camera(vec3 position, vec3 target) {
    mat4 view = mat4::look_at(position, target, VEC3_UP);

    shader_use(state.light_shader);
    shader_set_uniform_mat4(state.light_shader, "view", &view);
    shader_use(state.geometry_shader);
    shader_set_uniform_mat4(state.geometry_shader, "view", &view);
    shader_set_uniform_vec3(state.geometry_shader, "view_position", position);
    shader_use(state.model_shader);
    shader_set_uniform_mat4(state.model_shader, "view", &view);
    shader_set_uniform_vec3(state.model_shader, "view_position", position);
}

void renderer_render_light(vec3 position) {
    shader_use(state.light_shader);

    mat4 model = mat4::translate(position) * mat4::scale(vec3(0.1f));
    shader_set_uniform_mat4(state.light_shader, "model", &model);

    glBindVertexArray(state.cube_vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}