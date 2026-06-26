#include "nuklear_config.h"

#include "nuklear.h"
#include "nuklear_sdl_renderer.h"

#include <stdio.h>

#define WINDOWS_WIDTH 1200
#define WINDOW_HEIGHT 800

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

int main(void){
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *win = SDL_CreateWindow(
        "Conways's Game Of Life",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOWS_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN
    );
    
    SDL_Renderer *renderer = SDL_CreateRenderer(win, -1,  SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    struct nk_context *ctx = nk_sdl_init(win, renderer);

    struct nk_font_atlas *atlas;
    nk_sdl_font_stash_begin(&atlas);
    struct nk_font *font = nk_font_atlas_add_default(atlas, 13, 0);
    nk_sdl_font_stash_end();
    nk_style_set_font(ctx, &font->handle);

    int running = 1;

    while(running){
        SDL_Event evt;
        nk_input_begin(ctx);

        while(SDL_PollEvent(&evt)){
            if(evt.type == SDL_QUIT){
                running = 0;
            }

            nk_sdl_handle_event(&evt);
        }

        nk_input_end(ctx);

        displayGame(ctx);

        nk_end(ctx);

        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
        SDL_RenderClear(renderer);

        nk_sdl_render(NK_ANTI_ALIASING_ON);

        SDL_RenderPresent(renderer);
    }

    nk_sdl_shutdown();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(win);
    SDL_Quit();
}