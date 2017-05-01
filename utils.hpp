#ifndef _UTILS_H_
#define _UTILS_H_

#include <SDL2/SDL.h>
#include <cstdio>
#include <cstdint>

typedef int8_t s8;
typedef uint8_t u8;
typedef int16_t s16;
typedef uint16_t u16;
typedef int32_t s32;
typedef uint32_t u32;
typedef int64_t s64;
typedef uint64_t u64;


void force_quit(const char *str)
{
    puts(str);
    printf("Error: %s\n", SDL_GetError());
    SDL_Quit();
    exit(1);
}

#endif
