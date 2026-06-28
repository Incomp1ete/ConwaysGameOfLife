#define _POSIX_C_SOURCE 200809L

#include "GameOfLifeSimulation.h"
#include "assert.h"
#include <pthread.h>
#include <time.h>
#include <stdatomic.h>

bool **cellGrid;
bool **afterTickGrid;

pthread_t thread;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

atomic_bool paused = true;
bool running = false;

bool** mallocGrid(){
    bool** grid = malloc(VERTICAL_CELL_COUNT * sizeof(bool*));
    grid[0] = malloc(VERTICAL_CELL_COUNT * HORIZONTAL_CELL_COUNT + sizeof(bool));
    for(int i = 1; i < VERTICAL_CELL_COUNT; i++){
        grid[i] = grid[0] + i * HORIZONTAL_CELL_COUNT;
    }

    for(int y = 0; y < VERTICAL_CELL_COUNT; y++){
        for(int x = 0; x < HORIZONTAL_CELL_COUNT; x++){
            grid[y][x] = false;
        }
    }
    return grid;
}

void freeGrid(bool** grid){
    free(grid[0]);
    free(grid);
}

bool isSimulationPaused(){
    return atomic_load(&paused);
}

void pauseSimulation(void){
    pthread_mutex_lock(&mutex);
    atomic_store(&paused, true);
    pthread_mutex_unlock(&mutex);
}

void runSimulation(void){
    pthread_mutex_lock(&mutex);
    atomic_store(&paused, false);
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
}

void resetSimulation(void){
    pauseSimulation();
    afterTickGrid = mallocGrid();
    bool** previousGrid = cellGrid;
    cellGrid = afterTickGrid;
    free(previousGrid);
}

void *worker(void *arg){
    running = true;
    (void) arg;
    struct timespec next;
    clock_gettime(CLOCK_MONOTONIC, &next);

    while (running)
    {
        pthread_mutex_lock(&mutex);

        while (isSimulationPaused() && running)
        {
            pthread_cond_wait(&cond, &mutex);
        }

        pthread_mutex_unlock(&mutex);

        if (running == false)
        {
            break;
        }

        clock_gettime(CLOCK_MONOTONIC, &next);

        doOneTick();

        next.tv_sec += 1;

        clock_nanosleep(
            CLOCK_MONOTONIC,
            TIMER_ABSTIME,
            &next,
            NULL);
    }
    
    return NULL;
}

int getNeigbourCount(int x, int y){
    int neighbourCount = 0;

    int xLeft = x - 1;
    int xRight = x + 1;
    int yDown = y + 1;
    int yUp = y -1;

    bool notAtLeftEdge = x > 0;
    bool notAtRightEdge = x < HORIZONTAL_CELL_COUNT -1;
    bool notAtTopEdge = y > 0;
    bool notAtBottomEdge = y < VERTICAL_CELL_COUNT -1;

    if(notAtBottomEdge){
        if(cellGrid[yDown][x]){
            ++neighbourCount;
        }

        if(notAtLeftEdge && cellGrid[yDown][xLeft]){
            ++neighbourCount;
        }

        if(notAtRightEdge && cellGrid[yDown][xRight]){
            ++neighbourCount;
        }
    }

    if(notAtTopEdge){
        if(cellGrid[yUp][x]){
            ++neighbourCount;
        }

        if(notAtLeftEdge && cellGrid[yUp][xLeft]){
            ++neighbourCount;
        }

        if(notAtRightEdge && cellGrid[yUp][xRight]){
            ++neighbourCount;
        }
    }

    if(notAtRightEdge && cellGrid[y][xRight]){
        ++neighbourCount;
    }

    if(notAtLeftEdge && cellGrid[y][xLeft]){
        ++neighbourCount;
    }

    return neighbourCount;
}

void doOneTick(void){
    afterTickGrid = mallocGrid();
    for(int y = 0; y < VERTICAL_CELL_COUNT; y++){
        for(int x = 0; x < HORIZONTAL_CELL_COUNT; x++){
            int neighbourCount = getNeigbourCount(x, y);

            if(cellGrid[y][x]){
                if(neighbourCount < 2 || neighbourCount > 3){
                    afterTickGrid[y][x] = false;
                }
                else {
                    afterTickGrid[y][x] = true;
                }
            }
            else {
                if(neighbourCount == 3){
                    afterTickGrid[y][x] = true;
                }
                else {
                    afterTickGrid[y][x] = false;
                }
            }
        }
    }

    bool** previousGrid = cellGrid;
    cellGrid = afterTickGrid;
    free(previousGrid);
}

void initSimulation(void){
    cellGrid = mallocGrid();
    if(pthread_create(&thread, NULL, worker, NULL) != 0){
        assert(0 && "Failed  to create thread");
    }
}

void disposeSimulation(void){
    running = false;

    pthread_mutex_lock(&mutex);
    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&mutex);

    pthread_join(thread, NULL);
}