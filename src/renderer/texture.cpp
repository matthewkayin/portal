#include "texture.h"

#include "core/logger.h"
#include "core/resource.h"
#include <glad/glad.h>
#include <stb_image.h>
#include <string>
#include <unordered_map>

Texture texture_load(const char* path);

Texture texture_acquire(const char* path) {
    static std::unordered_map<std::string, Texture> textures;

    std::string full_path = resource_base_path + std::string(path);
    log_trace("Loading texture %s...", full_path.c_str());

    auto it = textures.find(full_path);
    if (it != textures.end()) {
        log_trace("Texture already loaded, returning copy.");
        return it->second;
    }

    Texture texture = texture_load(full_path.c_str());

    if (texture != 0) {
        textures[full_path] = texture;
        log_trace("Texture loaded successfully.");
    }

    return texture;
}

Texture texture_load(const char* path) {
    int width;
    int height;
    int number_of_components;
    stbi_uc* data = stbi_load(path, &width, &height, &number_of_components, 0);
    if (!data) {
        log_error("Could not load texture %s", path);
        return 0;
    }

    GLenum texture_format;
    if (number_of_components == 1) {
        texture_format = GL_RED;
    } else if (number_of_components == 3) {
        texture_format = GL_RGB;
    } else if (number_of_components == 4) {
        texture_format = GL_RGBA;
    } else {
        log_error("Texture format of texture %s not recognized.", path);
        return 0;
    }

    uint32_t texture;
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, texture_format, width, height, GL_FALSE, texture_format, GL_UNSIGNED_BYTE, data);
    // glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(data);

    return texture;
}

Texture texture_acquire_solidcolor(float r, float g, float b, float a) {
    static std::unordered_map<uint32_t, Texture> solidcolor_textures;

    // pack the color values into one byte
    uint32_t color = (uint32_t(a * 255.0f) << 24) | (uint32_t(b * 255.0f) << 16) | (uint32_t(g * 255.0f) << 8) | uint32_t(r * 255.0f);

    // Check if solid color texture of this color already exists
    auto it = solidcolor_textures.find(color);
    if (it != solidcolor_textures.end()) {
        return it->second;
    }

    // If it doesn't, create it
    Texture texture;
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, GL_FALSE, GL_RGBA, GL_UNSIGNED_BYTE, &color);

    // Since these are 1x1 textures there's no reason to use GL_LINEAR
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindTexture(GL_TEXTURE_2D, 0);

    solidcolor_textures[color] = texture;
    return texture;
}