#ifndef _UTILS_H_
#define _UTILS_H_

#include <SDL2/SDL.h>
#include <cstdio>

void force_quit(const char *str)
{
    puts(str);
    printf("Error: %s\n", SDL_GetError());
    SDL_Quit();
    exit(1);
}

#endif
