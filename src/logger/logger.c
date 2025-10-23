#include <stdio.h>
#include <time.h>
#include <cpro/logger.h>

static void stamp_now(char* buf, size_t n) {
    time_t t = time(NULL);
    struct tm tmv;
#if defined(_WIN32)
    localtime_s(&tmv, &t);
#else
    localtime_r(&t, &tmv);
#endif
    strftime(buf, n, "%Y-%m-%d %H:%M:%S", &tmv);
}

static void log_with(const char* level, const char* msg, FILE* out) {
    char ts[32]; stamp_now(ts, sizeof ts);
    fprintf(out, "[%s] %s: %s\n", ts, level, msg);
}

void log_error(const char* msg) { log_with("ERROR", msg, stderr); }
void log_info (const char* msg) { log_with("INFO",  msg, stderr); }
