#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <cassert>

// ---------------------
// Local includes
// ---------------------

#include "constants.h"
#include "types.h"

#include "math.cpp"
#include "debug.cpp"
#include "game.cpp"

// ---------------------
// Misc functions (should probably be in another module)
// ---------------------

// kills game with an error message
void force_quit(const char *str) {
    puts(str);
    printf("Error: %s\n", SDL_GetError());
    SDL_Quit();
    exit(1);
}

// ---------------------
// Main code
// ---------------------

input_t *initialize_input() {
    input_t *input = (input_t *) malloc(sizeof(*input));

    input->keys_pressed = SDL_GetKeyboardState(0);
    input->mouse.x = 0;
    input->mouse.y = 0;

    return input;
}

// entry point
int main(int, char *[]) {
    // initialize video
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        force_quit("Failed to initialize SDL.\n");
    }

    // create window
    SDL_Window* window = SDL_CreateWindow("SDL Template",
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          DEFAULT_SCREEN_WIDTH,
                                          DEFAULT_SCREEN_HEIGHT,
                                          SDL_WINDOW_SHOWN);
    if (!window) {
        force_quit("Failed to create window.\n");
    }

    // create renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        SDL_DestroyWindow(window);
        force_quit("Failed to create renderer.\n");
    }

    // enables vsync
    SDL_GL_SetSwapInterval(1); // TODO: find out why game runs so different with vsync off

    // initialization
    // TODO: set seed for RNG
    input_t *input = initialize_input();
    game_state_t *game_state = game_state_initialize(renderer);
    debug_initialize_text(renderer);

    // main loop
    bool running = true;
    uint64_t current_counter = SDL_GetPerformanceCounter();
    uint64_t last_counter = 0;
    double delta_time = 0;
    while (running) {
        // handle I/O events
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = false;
                    break;
                case SDL_MOUSEMOTION:
                    input->mouse.x = event.motion.x;
                    input->mouse.y = event.motion.y;
                    break;
                case SDL_KEYDOWN:
                    if (event.key.keysym.sym == SDLK_p) {
                        if (game_state->game_mode == PLAYING) {
                            game_state->game_mode = PAUSED;
                        } else if (game_state->game_mode == PAUSED) {
                            game_state->game_mode = PLAYING;
                        }
                    }
                    break;
            }
        }

        // calculate time elapsed since last frame
        last_counter = current_counter;
        current_counter = SDL_GetPerformanceCounter();
        // TODO: handle overflow
        delta_time = (double) (((current_counter - last_counter) * 1000) / SDL_GetPerformanceFrequency());

        // update world for this frame
        game_state_update(game_state, input, delta_time);

        // render frame
        game_state_render(game_state, renderer, delta_time);
        debug_draw_fps(delta_time);
        SDL_RenderPresent(renderer);
    }

    // clean up (needed mostly if the game is fullscreen, since screen resolution goes crazy)
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
