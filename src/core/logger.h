#pragma once

#ifndef LOG_LEVEL
#define LOG_LEVEL 3
#endif

enum LogLevel {
    LOG_LEVEL_ERROR = 0,
    LOG_LEVEL_WARN = 1,
    LOG_LEVEL_INFO = 2,
    LOG_LEVEL_TRACE = 3
};

bool logger_init();
void logger_quit();
void log_out(LogLevel level, const char* message, ...);

#define log_error(message, ...) log_out(LOG_LEVEL_ERROR, message, ##__VA_ARGS__);

#if LOG_LEVEL >= 1
#define log_warn(message, ...) log_out(LOG_LEVEL_WARN, message, ##__VA_ARGS__);
#else
#define log_warn(message, ...)
#endif

#if LOG_LEVEL >= 2
#define log_info(message, ...) log_out(LOG_LEVEL_INFO, message, ##__VA_ARGS__);
#else
#define log_info(message, ...)
#endif

#if LOG_LEVEL >= 3
#define log_trace(message, ...) log_out(LOG_LEVEL_TRACE, message, ##__VA_ARGS__);
#else
#define log_trace(message, ...)
#endif