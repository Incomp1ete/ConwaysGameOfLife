#include "GameOfLifeSimulation.h"
#include "assert.h"

bool **cellGrid;
bool **afterTickGrid;

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

void initSimulation(void){
    cellGrid = mallocGrid();
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

void stop(void){
    assert(0 && "Stop is not implemented");
}

void pause(void){
    assert(0 && "Pause is not implemented");
}

void play(void){
    assert(0 && "Play is not implemented");
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