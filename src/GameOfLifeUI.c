#include "GameOfLifeUI.h"

void displayGame(struct nk_context *ctx){
    if(nk_begin(ctx, "Game Of Life", nk_rect(0, 0, WINDOWS_WIDTH, WINDOW_HEIGHT),
    NK_WINDOW_BORDER | NK_WINDOW_MOVABLE)){
        nk_layout_row_begin(ctx, NK_STATIC, 30, 4);
        float buttonWidth = 30;

        nk_layout_row_push(ctx, buttonWidth);
        if(nk_button_symbol(ctx, NK_SYMBOL_RECT_SOLID)){
            // Stop
        }

        nk_layout_row_push(ctx, buttonWidth);
        if(nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_DOWN_OUTLINE)){
            // Pause
        }

        nk_layout_row_push(ctx, buttonWidth);
        if(nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_RIGHT)){
            // Play
        }

        nk_layout_row_push(ctx, buttonWidth);
        if(nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_RIGHT_OUTLINE)){
            // Frame
        }
    }
}