#ifndef _TEST_H_
#define _TEST_H_

#include <SDL2/SDL.h>
#include <cmath>
#include "custom_types.h"

typedef struct
{
    float red;
    float green;
    float blue;
    float alpha;
} color_t;

static color_t background_color;

void test_initialize()
{
    background_color.red = 0;
    background_color.green = 0;
    background_color.blue = 127;
    background_color.alpha = 255;
}

void test_update(double dt)
{
    static bool red_decreasing = false;
    if (red_decreasing)
    {
        background_color.red -= 0.1 * dt;
        if (background_color.red < 0) red_decreasing = false;
    }
    else
    {
        background_color.red += 0.1 * dt;
        if (background_color.red > 255) red_decreasing = true;
    }

    static bool blue_decreasing = false;
    if (blue_decreasing)
    {
        background_color.blue -= 0.1 * dt;
        if (background_color.blue < 0) blue_decreasing = false;
    }
    else
    {
        background_color.blue += 0.1 * dt;
        if (background_color.blue > 255) blue_decreasing = true;
    }
}

void test_render(SDL_Renderer *renderer, double)
{
    SDL_SetRenderDrawColor(renderer,
                           (u8) round(background_color.red),
                           (u8) round(background_color.green),
                           (u8) round(background_color.blue),
                           (u8) round(background_color.alpha));
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
}

#endif
