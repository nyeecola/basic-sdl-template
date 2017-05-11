#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include <GL/gl.h>

#include <string>
#include <list>
#include <map>

#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <cassert>

#include "bin_heap.hpp"
#include "constants.hpp"
#include "math.hpp"
#include "game_mode.hpp"
#include "menu_mode.hpp"
#include "game_state.hpp"
#include "main.hpp"

#include "bin_heap.cpp"
#include "math.cpp"
#include "utils.cpp"
#include "debug.cpp"
#include "menu_mode.cpp"
#include "game_mode.cpp"
#include "game_state.cpp"

int main(int, char *[])
{
    // initialize video
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        force_quit("Failed to initialize SDL.");
    }

    // create window
    SDL_Window* window = SDL_CreateWindow("Bullet Hell",
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          DEFAULT_SCREEN_WIDTH,
                                          DEFAULT_SCREEN_HEIGHT,
                                          SDL_WINDOW_SHOWN);
    if (!window)
    {
        force_quit("Failed to create window.");
    }

    // TODO: handle error (should just print a message, not crash the program)
#ifdef GPU
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 16);
    glEnable(GL_MULTISAMPLE_ARB);
#endif

    // create renderer
#ifdef GPU
    SDL_Renderer* sdl_renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
#else
    SDL_Renderer* sdl_renderer = SDL_CreateRenderer(window, -1, 0);
#endif
    puts("----------------------");
    puts("Video drivers available:");
    int num_drivers = SDL_GetNumVideoDrivers();
    for (int i = 0; i < num_drivers; i++)
    {
        SDL_RendererInfo info;
        SDL_GetRenderDriverInfo(i, &info);

        printf("- %s\n", SDL_GetVideoDriver(i));
        printf("--- name:  %s\n", info.name);
        printf("--- flags: 0x%08x\n", info.flags);
        if (info.flags & SDL_RENDERER_ACCELERATED) puts("----- HARDWARE");
        if (info.flags & SDL_RENDERER_SOFTWARE) puts("----- SOFTWARE");
        if (info.flags & SDL_RENDERER_PRESENTVSYNC) puts("----- VSYNC");
        if (info.flags & SDL_RENDERER_TARGETTEXTURE) puts("----- TEXTURES");
    }
    puts("----------------------");
    puts("----------------------");
    SDL_RendererInfo info;
    int err = SDL_GetRendererInfo(sdl_renderer, &info);
    if (err) puts("Error getting renderer info.");
    else
    {
        printf("Currently using:\n");
        printf("--- name:  %s\n", info.name);
        printf("--- flags: 0x%08x\n", info.flags);
        if (info.flags & SDL_RENDERER_ACCELERATED) puts("----- HARDWARE");
        if (info.flags & SDL_RENDERER_SOFTWARE) puts("----- SOFTWARE");
        if (info.flags & SDL_RENDERER_PRESENTVSYNC) puts("----- VSYNC");
        if (info.flags & SDL_RENDERER_TARGETTEXTURE) puts("----- TEXTURES");
    }
    puts("----------------------");
    if (!sdl_renderer)
    {
        SDL_DestroyWindow(window);
        force_quit("Failed to create renderer.");
    }
    renderer_t renderer = {};
    renderer.sdl = sdl_renderer;

    // initialization
    // TODO: set seed for RNG
    debug_initialize_text(&renderer);
    input_t *input = initialize_input();
    game_state_t *game_state = (game_state_t *) calloc(1, sizeof(*game_state));
    assert(game_state);
    game_state->game = initialize_game_mode();
    game_state->menu = initialize_menu_mode();
    game_state->type = GAME_MODE;
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
                case SDL_KEYDOWN:
                    if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
                    {
                        if (game_state->type == MENU_MODE)
                        {
                            game_state->type = GAME_MODE;
                        }
                        else if (game_state->type == GAME_MODE)
                        {
                            game_state->type = MENU_MODE;
                        }
                        break;
                    }
                default: break;
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

    // TODO: stop using free here (and make cppcheck stop complaining)
    free(game_state);
    SDL_DestroyRenderer(renderer.sdl);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
