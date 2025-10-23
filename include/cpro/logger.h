#ifndef CPRO_LOGGER_H
#define CPRO_LOGGER_H
#ifdef __cplusplus
extern "C" {
#endif

    void log_error(const char* msg);
    void log_info(const char* msg);

#ifdef __cplusplus
}
#endif
#endif /* CPRO_LOGGER_H */

