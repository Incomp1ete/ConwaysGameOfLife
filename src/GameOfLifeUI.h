#ifndef GAME_OF_LIFE_UI_H
#define GAME_OF_LIFE_UI_H

#include "nuklear_config.h"
#include "nuklear.h"

#define WINDOWS_WIDTH 1200
#define WINDOW_HEIGHT 800

void initUi(void);
void disposeUi(void);
void displayGame(struct nk_context* ctx);

#endif