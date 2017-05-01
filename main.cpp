#include <SDL2/SDL.h>
#include <cstdio>
#include <cstdlib>
#include <cmath>

#include "utils.hpp"
#include "constants.hpp"

#include "test.hpp"

void initialize(SDL_Renderer *renderer, const u8 *keys_pressed)
{
    test_initialize(renderer, keys_pressed);
}

void update(double dt)
{
    test_update(dt);
}

void render(SDL_Renderer *renderer, double dt)
{
    test_render(renderer, dt);
}

void handle(SDL_Event)
{

}

int main(int, char *[])
{
    // initialize video
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        force_quit("Failed to initialize SDL.\n");
    }

    // create window
    SDL_Window* window = SDL_CreateWindow("SDL Tutorial",
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
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer)
    {
        SDL_DestroyWindow(window);
        force_quit("Failed to create renderer.\n");
    }

    // initialization
    const u8 *keys_pressed = SDL_GetKeyboardState(0);
    initialize(renderer, keys_pressed);

    // main loop
    bool running = true;
    u64 current_counter = SDL_GetPerformanceCounter();
    u64 last_counter = 0;
    double delta_time = 0;
    while (running)
    {
        // handle events
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                running = false;
            }
            else
            {
                handle(event);
            }
        }

        // calculate time elapsed since last frame
        last_counter = current_counter;
        current_counter = SDL_GetPerformanceCounter();
        delta_time = (double) ((abs(current_counter - last_counter) * 1000) / SDL_GetPerformanceFrequency());

        // update world for this frame
        update(delta_time);

        // render frame
        render(renderer, delta_time);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
