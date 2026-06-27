#include "PerformanceMeasuring.h"
#include <time.h>

clock_t startTime;
clock_t timeDiff;

void startTimer(void){
    startTime = clock();
}

void takeMeasurement(void){
    timeDiff = clock() - startTime;
}

float getLastMeasurement(void){
    return (float)timeDiff / CLOCKS_PER_SEC;
}