#include <SDL2/SDL.h>
#include <cstdio>
#include <cstdlib>
#include <cmath>

#include "custom_types.hpp"
#include "utils.hpp"
#include "debug.hpp"

#include "test.hpp"

const int DEFAULT_SCREEN_WIDTH = 800;
const int DEFAULT_SCREEN_HEIGHT = 600;

void initialize(SDL_Renderer *renderer)
{
    debug_initialize_text(renderer);

    test_initialize();
}

void update(double dt)
{
    test_update(dt);
}

void render(SDL_Renderer *renderer, double dt)
{
    test_render(renderer, dt);

    debug_draw_fps(dt);

    SDL_RenderPresent(renderer);
}

int main(int, char *[])
{
    SDL_Window* window;
    SDL_Renderer* renderer;

    // initialize video
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        force_quit("Failed to initialize SDL.\n");
    }

    // create window
    window = SDL_CreateWindow("SDL Tutorial",
                              SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED,
                              DEFAULT_SCREEN_WIDTH,
                              DEFAULT_SCREEN_HEIGHT,
                              SDL_WINDOW_SHOWN);
    if (!window)
    {
        force_quit("Failed to create window.\n");
    }

    // create renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer)
    {
        SDL_DestroyWindow(window);
        force_quit("Failed to create renderer.\n");
    }

    // initialization
    initialize(renderer);

    // main loop
    bool running = true;
    u64 current_counter = SDL_GetPerformanceCounter();
    u64 last_counter = 0;
    double delta_time = 0;
    SDL_Event event;
    while (running)
    {
        // handle events
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT) running = false;
        }

        // calculate time elapsed since last frame
        last_counter = current_counter;
        current_counter = SDL_GetPerformanceCounter();
        delta_time = (double) ((abs(current_counter - last_counter) * 1000) / SDL_GetPerformanceFrequency());

        // update world for this frame
        update(delta_time);

        // render frame
        render(renderer, delta_time);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
