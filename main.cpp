#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include <string>
#include <list>
#include <map>

#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <cassert>

#include "constants.hpp"
#include "main.hpp"

#include "utils.cpp"
#include "math.cpp"
#include "debug.cpp"
#include "game.cpp"

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
#if 1
    SDL_Renderer* sdl_renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
#else
    SDL_Renderer* sdl_renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
#endif
    if (!sdl_renderer)
    {
        SDL_DestroyWindow(window);
        force_quit("Failed to create renderer.\n");
    }
    renderer_t renderer = {};
    renderer.sdl = sdl_renderer;

    // initialization
    // set seed for RNG
    input_t *input = initialize_input();
    game_state_t *game_state = game_state_initialize();
    debug_initialize_text(&renderer);

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
            switch (event.type)
            {
                case SDL_QUIT:
                    running = false;
                    break;
                case SDL_MOUSEMOTION:
                    input->mouse.x = event.motion.x;
                    input->mouse.y = event.motion.y;
                    break;
            }
        }

        // calculate time elapsed since last frame
        last_counter = current_counter;
        current_counter = SDL_GetPerformanceCounter();
        delta_time = (double) ((abs(current_counter - last_counter)) /
                                (double) SDL_GetPerformanceFrequency());

        // update world for this frame
        game_state_update(game_state, input, delta_time);

        // render frame
        game_state_render(game_state, &renderer);
        debug_draw_fps(delta_time);
        SDL_RenderPresent(renderer.sdl);
    }

    SDL_DestroyRenderer(renderer.sdl);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
