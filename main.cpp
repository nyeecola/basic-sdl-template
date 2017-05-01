#include <SDL2/SDL.h>
#include <cstdio>
#include <cstdlib>
#include <cmath>

#include "custom_types.h"
#include "utils.h"

#include "test.h"

const int DEFAULT_SCREEN_WIDTH = 800;
const int DEFAULT_SCREEN_HEIGHT = 600;

void initialize()
{
    test_initialize();
}

void update(double dt)
{
    test_update(dt);
}

void render(SDL_Renderer *renderer, double dt)
{
    test_render(renderer, dt);
}

int main(int, char *[])
{
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;

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
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer)
    {
        SDL_DestroyWindow(window);
        force_quit("Failed to create renderer.\n");
    }

    // initialization
    initialize();

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
        delta_time = (double) (abs(current_counter - last_counter) * 1000 / SDL_GetPerformanceFrequency());

        // update world for this frame
        update(delta_time);

        // render frame
        render(renderer, delta_time);
    }

    // probably not really needed for windows and linux
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
