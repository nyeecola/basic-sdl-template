#include <SDL.h>
#include <cstdio>
#include <cstdlib>

const int DEFAULT_SCREEN_WIDTH = 800;
const int DEFAULT_SCREEN_HEIGHT = 600;

int main(int argc, char *args[])
{
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;

    // initialize video
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("Failed to initialize SDL: %s\n", SDL_GetError());
        SDL_Quit();
        exit(1);
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
        printf("Failed to create window: %s\n", SDL_GetError());
        SDL_Quit();
        exit(1);
    }

    // create renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer)
    {
        printf("Failed to create renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        exit(1);
    }

    // main loop
    bool running = true;
    SDL_Event event;
    while (running)
    {
        // handle events
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT) running = false;
        }

        // TODO: application logic

        // TODO: render something
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xff);
        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);
    }

    // probably not really needed for windows and linux
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
