#ifndef CLOCK_H
#define CLOCK_H

typedef struct Timer Timer;
int timer_init(Timer** timer, double waitInSeconds);
int timer_restart(Timer* timer);
int timer_sleepTillDeadline(Timer* timer);
void timer_destroy(Timer* timer);

#endif
