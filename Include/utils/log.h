#ifndef __LOG_H__
#define __LOG_H__

#ifndef __VT_COLOR__
#define __VT_COLOR__
#define RED "\033[31m"
#define YELLOW "\033[33m"
#define NONE "\033[0m"
#endif /* __VT_COLOR__ */

#include <time.h>
#include <errno.h>

#define LOG(level, fmt, ...) \
    do { \
        time_t now = time(NULL); \
        char buf[20]; \
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&now)); \
        fprintf(stderr, "[%s] [%s] " fmt "\n", buf, level, ##__VA_ARGS__); \
    } while (0)

#define ERROR(fmt, args...) LOG(RED"ERROR"NONE, fmt, ##args)
#define WARN(fmt, args...) LOG(YELLOW"WARN"NONE, fmt, ##args)
#define INFO(fmt, args...) LOG("INFO", fmt, ##args)

#endif /* __LOG_H__ */