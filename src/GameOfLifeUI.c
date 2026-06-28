#include "GameOfLifeUI.h"
#include "GameOfLifeSimulation.h"
#include "PerformanceMeasuring.h"

void renderToolbar(struct nk_context *ctx){
        nk_layout_row_begin(ctx, NK_STATIC, 30, 5);
        const float buttonWidth = 30;

        nk_layout_row_push(ctx, buttonWidth);
        if(nk_button_symbol(ctx, NK_SYMBOL_RECT_SOLID)){
            resetSimulation();
        }

        nk_layout_row_push(ctx, buttonWidth);
        if(nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_DOWN_OUTLINE)
            && isSimulationPaused() == false){
            pauseSimulation();
        }

        nk_layout_row_push(ctx, buttonWidth);
        if(nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_RIGHT)
            && isSimulationPaused()){
            runSimulation();
        }

        nk_layout_row_push(ctx, buttonWidth);
        if(nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_RIGHT_OUTLINE) && isSimulationPaused()){
            doOneTick();
        }

        nk_layout_row_push(ctx, 4*buttonWidth);
        float measurement = getLastMeasurement();
        nk_labelf(ctx, NK_TEXT_LEFT, "Frametime: %f", measurement);
}

void renderCellGrid(struct nk_context *ctx){
    const float cell_size = 16.0f;
    struct nk_rect area;

    nk_layout_row_dynamic(ctx, VERTICAL_CELL_COUNT * cell_size, 1);
    if(nk_widget(&area, ctx)){
        struct nk_command_buffer *canvas = nk_window_get_canvas(ctx);

        for(int y = 0; y < VERTICAL_CELL_COUNT; y++){
            for(int x = 0; x < HORIZONTAL_CELL_COUNT; x++){
                struct nk_rect cell = nk_rect(
                    area.x + x * cell_size,
                    area.y + y * cell_size,
                    cell_size,
                    cell_size
                );

                nk_fill_rect(
                    canvas,
                    cell,
                    0,
                    cellGrid[y][x] ? nk_rgb(0, 255, 0) 
                    : nk_rgb(30, 30, 30)
                );

                nk_stroke_rect(
                    canvas,
                    cell,
                    0,
                    1,
                    nk_rgb(80,80,80)
                );

                if(nk_input_is_mouse_click_in_rect(&ctx->input, NK_BUTTON_LEFT, cell))
                {
                    cellGrid[y][x] ^= 1;
                }
            }
        }
    }
}

void displayGame(struct nk_context *ctx){
    startTimer();
    if(nk_begin(ctx, "Game Of Life", nk_rect(0, 0, WINDOWS_WIDTH, WINDOW_HEIGHT),
    NK_WINDOW_BORDER | NK_WINDOW_MOVABLE)){
        renderToolbar(ctx);
        renderCellGrid(ctx);
    }
    takeMeasurement();
}