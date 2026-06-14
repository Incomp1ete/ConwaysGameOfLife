#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_DEFAULT_FONT

#define NK_IMPLEMENTATION
#include "nuklear.h"

#define NK_SDL_RENDERER_IMPLEMENTATION
#include "nuklear_sdl_renderer.h"

#include <stdio.h>

#define WINDOWS_WIDTH 1200
#define WINDOW_HEIGHT 800

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

displayGame(struct nk_context *ctx){
    if(nk_begin(ctx, "Game Of Life", nk_rect(0, 0, WINDOWS_WIDTH, WINDOW_HEIGHT),
    NK_WINDOW_BORDER | NK_WINDOW_MOVABLE)){
        nk_layout_row_dynamic(ctx, 30, 1);
        nk_label(ctx, "Hello World", NK_TEXT_LEFT);
    }
}