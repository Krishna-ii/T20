#include <time.h>
#include "../../include/utils/time_utils.h"

long get_time_seconds(void)
{
    return (long)time(NULL);
}

long get_time_ms(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (long)(ts.tv_sec * 1000L + ts.tv_nsec / 1000000L);
}
