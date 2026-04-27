#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <pthread.h>
#include "logger.h"

static FILE *g_log_file = NULL;
static pthread_mutex_t g_log_lock = PTHREAD_MUTEX_INITIALIZER;

static const char* level_to_str(LogLevel level) {
    switch (level) {
        case LOG_INFO: return "INFO";
        case LOG_WARN: return "WARN";
        case LOG_ERROR: return "ERROR";
        case LOG_DEBUG: return "DEBUG";
        default: return "UNKNOWN";
    }
}

int logger_init(const char *path) {
    pthread_mutex_lock(&g_log_lock);
    g_log_file = fopen(path, "w");
    pthread_mutex_unlock(&g_log_lock);
    return g_log_file ? 0 : -1;
}

void logger_close(void) {
    pthread_mutex_lock(&g_log_lock);
    if (g_log_file) {
        fclose(g_log_file);
        g_log_file = NULL;
    }
    pthread_mutex_unlock(&g_log_lock);
}

void log_msg(LogLevel level, const char *fmt, ...) {
    pthread_mutex_lock(&g_log_lock);

    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    char time_buf[32];
    strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S", tm_info);

    fprintf(stdout, "[%s] [%s] ", time_buf, level_to_str(level));

    if (g_log_file) {
        fprintf(g_log_file, "[%s] [%s] ", time_buf, level_to_str(level));
    }

    va_list args;
    va_start(args, fmt);
    vfprintf(stdout, fmt, args);
    va_end(args);

    if (g_log_file) {
        va_start(args, fmt);
        vfprintf(g_log_file, fmt, args);
        va_end(args);
        fprintf(g_log_file, "\n");
        fflush(g_log_file);
    }

    fprintf(stdout, "\n");
    fflush(stdout);
    pthread_mutex_unlock(&g_log_lock);
}
