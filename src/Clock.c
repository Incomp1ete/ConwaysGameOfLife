#include "Clock.h"
#include <stdlib.h>
#include <time.h>

struct Timer {
    struct timespec start;
    double waitInSeconds;
};

int timer_init(Timer** timer, double waitInSeconds){
    *timer = malloc(sizeof(Timer));
    if(*timer == NULL){
        return -1;
    }

    (*timer)->waitInSeconds = waitInSeconds;

    return clock_gettime(CLOCK_MONOTONIC, &((*timer)->start));
}

int timer_restart(Timer* timer){
    return clock_gettime(CLOCK_MONOTONIC, &(timer->start));
}

int timer_sleepTillDeadline(Timer* timer){
    struct timespec end;
    int ret = clock_gettime(CLOCK_MONOTONIC, &end);

    if(ret != 0){
        return ret;
    }

    double elapsed = (end.tv_sec - timer->start.tv_sec)
                        + (end.tv_nsec - timer->start.tv_nsec) / 1e9;

    double remaining = timer->waitInSeconds - elapsed;

    if(remaining > 0){
        struct timespec sleep;
        sleep.tv_sec = (time_t)remaining;
        sleep.tv_nsec = (long)((remaining -sleep.tv_sec) * 1e9);
        ret = nanosleep(&sleep, NULL);
    }

    return ret;
}

void timer_destroy(Timer* timer){
    free(timer);
}