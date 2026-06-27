#ifndef GAME_OF_LIFE_SIMULATION_H
#define GAME_OF_LIFE_SIMULATION_H

#include <stdbool.h>
#include <stdlib.h>

#define HORIZONTAL_CELL_COUNT 50
#define VERTICAL_CELL_COUNT 50

void initSimulation(void);
void stop(void);
void pause(void);
void play(void);
void doOneTick(void);

extern bool **cellGrid;

#endif