#pragma once

#include <cstdint>

typedef uint32_t Texture;

Texture texture_acquire(const char* path);
Texture texture_acquire_solidcolor(float r, float g, float b, float a);