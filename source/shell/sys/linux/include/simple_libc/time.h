#ifndef __SIMPLE_LIBC_TIME_H__
#define __SIMPLE_LIBC_TIME_H__

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED && VSF_LINUX_USE_SIMPLE_LIBC == ENABLED
#   include "./stddef.h"
#else
#   include <stddef.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if VSF_LINUX_LIBC_CFG_FAKE_API == ENABLED
#define clock_gettime       __vsf_linux_clock_gettime
#define nanosleep           __vsf_linux_nanosleep
#define clock               __vsf_linux_clock
#endif

#ifndef __TIME_T
#   define __TIME_T         long long
#endif
typedef __TIME_T            time_t;

#ifndef __CLOCK_T
#   define __CLOCK_T        long
#endif
typedef __CLOCK_T           clock_t;

#define CLOCKS_PER_SEC      ((clock_t)1000)

struct tm {
    int tm_sec;
    int tm_min;
    int tm_hour;
    int tm_mday;
    int tm_mon;
    int tm_year;
    int tm_wday;
    int tm_yday;
    int tm_isdst;
    long int tm_gmtoff;
    const char *tm_zone;
};

struct timespec {
    time_t  tv_sec;
    long    tv_nsec;
};

typedef enum {
    CLOCK_MONOTONIC,
    CLOCK_REALTIME,
} clockid_t;

clock_t clock(void);
int clock_gettime(clockid_t clk_id, struct timespec *tp);

time_t time(time_t *t);
char *asctime(const struct tm *tm);
char *ctime(const time_t *t);
double difftime(time_t time1, time_t time2);
struct tm *gmtime(const time_t *t);
struct tm *localtime(const time_t *t);
time_t mktime(struct tm *tm);
size_t strftime(char *str, size_t maxsize, const char *format, const struct tm *tm);

int nanosleep(const struct timespec *requested_time, struct timespec *remaining);

#ifdef __cplusplus
}
#endif

#endif
