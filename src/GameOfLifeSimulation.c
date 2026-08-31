#include "GameOfLifeSimulation.h"
#include "assert.h"
#include "Threading.h"
#include "clock.h"
#include <stdatomic.h>

bool **cellGrid;
bool **afterTickGrid;

Thread* thread;

Mutex* pauseMutex;;
Cond* pauseCond;
atomic_bool paused = true;

Mutex* gridMutex;

bool keepRunning = false;

bool** mallocGrid(){
    bool** grid = malloc(VERTICAL_CELL_COUNT * sizeof(bool*));
    grid[0] = malloc(VERTICAL_CELL_COUNT * HORIZONTAL_CELL_COUNT * sizeof(bool));
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

void lockGrid(void){
    mutex_lock(gridMutex);
}

void unlockGrid(void){
    mutex_unlock(gridMutex);
}

bool isSimulationPaused(void){
    return atomic_load(&paused);
}

void pauseSimulation(void){
    mutex_lock(pauseMutex);
    atomic_store(&paused, true);
    mutex_unlock(pauseMutex);
}

void runSimulation(void){
    mutex_lock(pauseMutex);
    atomic_store(&paused, false);
    cond_signal(pauseCond);
    mutex_unlock(pauseMutex);
}

void resetSimulation(void){
    pauseSimulation();
    lockGrid();
    afterTickGrid = mallocGrid();
    bool** previousGrid = cellGrid;
    cellGrid = afterTickGrid;
    free(previousGrid);
    unlockGrid();
}

void *worker(void *arg){
    (void) arg;
    struct Timer* timer;
    timer_init(&timer, 1.0);

    while (true)
    {
        mutex_lock(pauseMutex);
        while (isSimulationPaused() && keepRunning)
        {
            cond_wait(pauseCond, pauseMutex);
        }

        if (keepRunning == false)
        {
            mutex_unlock(pauseMutex);
            break;
        }

        mutex_unlock(pauseMutex);

        timer_restart(timer);
        doOneTick();
        timer_sleepTillDeadline(timer);
    }

    timer_destroy(timer);
    timer = NULL;
    
    return NULL;
}

int getNeighbourCount(int x, int y){
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
    mutex_lock(gridMutex);
    // ToDo: Double Buffer
    afterTickGrid = mallocGrid();
    for(int y = 0; y < VERTICAL_CELL_COUNT; y++){
        for(int x = 0; x < HORIZONTAL_CELL_COUNT; x++){
            int neighbourCount = getNeighbourCount(x, y);

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
    mutex_unlock(gridMutex);
}

void initSimulation(void){
    cellGrid = mallocGrid();
    mutex_create(&pauseMutex);
    cond_create(&pauseCond);
    mutex_create(&gridMutex);
    keepRunning = true;
    if(thread_create(&thread, worker)){
        assert(0 && "Failed  to create thread");
    }
}

void disposeSimulation(void){
    mutex_lock(pauseMutex);
    keepRunning = false;
    cond_broadcast(pauseCond);
    mutex_unlock(pauseMutex);

    thread_join(thread);
    thread = NULL;

    mutex_destroy(pauseMutex);
    pauseMutex = NULL;

    cond_destroy(pauseCond);
    pauseCond = NULL;

    mutex_destroy(gridMutex);
    gridMutex = NULL;
}