#include "Clock.h"
#include <stdlib.h>

#if defined(_WIN32)
#include <windows.h>

struct Timer
{
    LARGE_INTEGER freq;
    LARGE_INTEGER start;
    LARGE_INTEGER end;
    double waitInMs;
};

int timer_init(Timer **timer, double waitInSeconds)
{
    *timer = malloc(sizeof(Timer));
    if (*timer == NULL)
    {
        return -1;
    }

    QueryPerformanceFrequency(&((*timer)->freq));
    timeBeginPeriod((int)waitInSeconds);
    (*timer)->waitInMs = 1000.0 * waitInSeconds;
    return 0;
}

int timer_restart(Timer *timer)
{
    QueryPerformanceCounter(&(timer->start));
    return 0;
}

int timer_sleepTillDeadline(Timer *timer)
{
    LARGE_INTEGER end;
    QueryPerformanceCounter(&end);

    double elapsedMs = (double)(end.QuadPart - timer->start.QuadPart) * 1000.0 / timer->freq.QuadPart;
    double remainingMs = (timer->waitInMs) - elapsedMs;

    if (remainingMs > 0)
    {
        Sleep((DWORD)remainingMs);
    }

    return 0;
}

void timer_destroy(Timer *timer)
{
    timeEndPeriod((int)(timer->waitInMs) / 1000);
    free(timer);
}
#elif defined(__unix__)
#include <unistd.h> // Needed for _POSIX_VERSION guard

#if defined(_POSIX_VERSION)
#include <time.h>

struct Timer
{
    struct timespec start;
    double waitInSeconds;
};

int timer_init(Timer **timer, double waitInSeconds)
{
    *timer = malloc(sizeof(Timer));
    if (*timer == NULL)
    {
        return -1;
    }

    (*timer)->waitInSeconds = waitInSeconds;

    return clock_gettime(CLOCK_MONOTONIC, &((*timer)->start));
}

int timer_restart(Timer *timer)
{
    return clock_gettime(CLOCK_MONOTONIC, &(timer->start));
}

int timer_sleepTillDeadline(Timer *timer)
{
    struct timespec end;
    int ret = clock_gettime(CLOCK_MONOTONIC, &end);

    if (ret != 0)
    {
        return ret;
    }

    double elapsed = (end.tv_sec - timer->start.tv_sec) + (end.tv_nsec - timer->start.tv_nsec) / 1e9;

    double remaining = timer->waitInSeconds - elapsed;

    if (remaining > 0)
    {
        struct timespec sleep;
        sleep.tv_sec = (time_t)remaining;
        sleep.tv_nsec = (long)((remaining - sleep.tv_sec) * 1e9);
        ret = nanosleep(&sleep, NULL);
    }

    return ret;
}

void timer_destroy(Timer *timer)
{
    free(timer);
}

#else

#error "unsupported platform"

#endif

#else

#error "unsupported platform"

#endif