#ifndef LOGGER_H
#define LOGGER_H

typedef enum {
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR,
    LOG_DEBUG
} LogLevel;

int logger_init(const char *path);
void logger_close(void);
void log_msg(LogLevel level, const char *fmt, ...);

#endif
