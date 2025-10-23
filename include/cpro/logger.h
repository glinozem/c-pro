#ifndef LOGGER_H
#define LOGGER_H

#ifdef __cplusplus
extern "C" {
#endif

    void log_error(const char* msg);
    void log_info(const char* msg);

#ifdef __cplusplus
}
#endif
#endif /* LOGGER_H */
