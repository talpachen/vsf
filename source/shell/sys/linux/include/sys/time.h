#ifndef __VSF_LINUX_SYS_TIME_H__
#define __VSF_LINUX_SYS_TIME_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED && VSF_LINUX_USE_SIMPLE_TIME == ENABLED
#   include "../simple_libc/time.h"
#else
#   include <time.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define getitimer           __vsf_linux_getitimer
#define setitimer           __vsf_linux_setitimer
#define gettimeofday        __vsf_linux_gettimeofday

#ifndef __SUSECONDS_T
#   define __SUSECONDS_T    long int
#endif
typedef __SUSECONDS_T       suseconds_t;

struct timeval {
    time_t          tv_sec;
    suseconds_t     tv_usec;
};

struct timezone {
    int             tz_minuteswest;
    int             tz_dsttime;
};

struct itimerval {
    struct timeval  it_value;
    struct timeval  it_interval;
};

int gettimeofday(struct timeval * tv, struct timezone * tz);
int getitimer(int which, struct itimerval *curr_value);
int setitimer(int which, const struct itimerval *new_valie, struct itimerval *old_value);

#ifdef __cplusplus
}
#endif

#endif
