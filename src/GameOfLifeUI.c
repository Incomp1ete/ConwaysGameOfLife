#define _POSIX_C_SOURCE 200809L

#include "GameOfLifeUI.h"
#include "GameOfLifeSimulation.h"
#include "PerformanceMeasuring.h"
#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include <stdatomic.h>

pthread_t thread;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

atomic_bool paused = true;
bool running = false;

bool isPaused(){
    return atomic_load(&paused);
}

void *worker(void *arg){
    running = true;
    (void) arg;
    struct timespec next;
    clock_gettime(CLOCK_MONOTONIC, &next);

    while (running)
    {
        pthread_mutex_lock(&mutex);

        while (isPaused() && running)
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

void initUi(void){
    if(pthread_create(&thread, NULL, worker, NULL) != 0){
        assert(0 && "Failed  to create thread");
    }
}

void disposeUi(void){
    running = false;

    pthread_mutex_lock(&mutex);
    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&mutex);

    pthread_join(thread, NULL);
}

void pauseSimulation(void){
    pthread_mutex_lock(&mutex);
    atomic_store(&paused, true);
    pthread_mutex_unlock(&mutex);
}

void playSimulation(void){
    pthread_mutex_lock(&mutex);
    atomic_store(&paused, false);
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
}

void renderToolbar(struct nk_context *ctx){
        nk_layout_row_begin(ctx, NK_STATIC, 30, 5);
        const float buttonWidth = 30;

        nk_layout_row_push(ctx, buttonWidth);
        if(nk_button_symbol(ctx, NK_SYMBOL_RECT_SOLID)){
            atomic_store(&paused, true);
            reset();
        }

        nk_layout_row_push(ctx, buttonWidth);
        if(nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_DOWN_OUTLINE)
            && isPaused() == false){
            pauseSimulation();
        }

        nk_layout_row_push(ctx, buttonWidth);
        if(nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_RIGHT)
            && isPaused()){
            playSimulation();
        }

        nk_layout_row_push(ctx, buttonWidth);
        if(nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_RIGHT_OUTLINE) && isPaused()){
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