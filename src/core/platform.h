#pragma once

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#define PLATFORM_WIN32 1
#ifndef _WIN64
#error "64-bit is required on Windows!"
#elif __APPLE__
#define PLATFORM_OSX 1
#endif
#endif