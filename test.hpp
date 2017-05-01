#ifndef _TEST_H_
#define _TEST_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <cmath>

#include "utils.hpp"
#include "debug.hpp"
#include "constants.hpp"

static const u8 *global_keys_pressed = 0;

typedef struct
{
    float red;
    float green;
    float blue;
    float alpha;
} color_t;

static color_t background_color;

typedef struct
{
    double x;
    double y;
    double speed;

    SDL_Texture *image_path;
    int w;
    int h;
} ball_t;

static ball_t global_ball;

void test_initialize(SDL_Renderer *renderer, const u8 *keys_pressed)
{
    global_ball.image_path = IMG_LoadTexture(renderer, BALL_IMG_PATH);
    SDL_QueryTexture(global_ball.image_path, 0, 0, &global_ball.w, &global_ball.h);
    global_ball.speed = BALL_SPEED;

    debug_initialize_text(renderer);

    global_keys_pressed = keys_pressed;

    background_color.red = 127;
    background_color.green = 0;
    background_color.blue = 127;
    background_color.alpha = 255;
}

void test_update(double dt)
{
    if (global_keys_pressed[SDL_SCANCODE_UP])
    {
        global_ball.y -= global_ball.speed * dt;
    }
    if (global_keys_pressed[SDL_SCANCODE_DOWN])
    {
        global_ball.y += global_ball.speed * dt;
    }
    if (global_keys_pressed[SDL_SCANCODE_LEFT])
    {
        global_ball.x -= global_ball.speed * dt;
    }
    if (global_keys_pressed[SDL_SCANCODE_RIGHT])
    {
        global_ball.x += global_ball.speed * dt;
    }

    double multiplier = 0.1;
    static bool red_decreasing = false;
    if (red_decreasing)
    {
        background_color.red -= multiplier * dt;
        if (background_color.red < 5) red_decreasing = false;
    }
    else
    {
        background_color.red += multiplier * dt;
        if (background_color.red > 250) red_decreasing = true;
    }

    static bool blue_decreasing = false;
    if (blue_decreasing)
    {
        background_color.blue -= multiplier * dt;
        if (background_color.blue < 5) blue_decreasing = false;
    }
    else
    {
        background_color.blue += multiplier * dt;
        if (background_color.blue > 250) blue_decreasing = true;
    }
}

void test_render(SDL_Renderer *renderer, double dt)
{
    SDL_SetRenderDrawColor(renderer,
                           (u8) round(background_color.red),
                           (u8) round(background_color.green),
                           (u8) round(background_color.blue),
                           (u8) round(background_color.alpha));
    SDL_RenderClear(renderer);

    SDL_Rect ball_rect;
    ball_rect.x = round(global_ball.x);
    ball_rect.y = round(global_ball.y);
    ball_rect.w = round(BALL_SCALE * global_ball.w);
    ball_rect.h = round(BALL_SCALE * global_ball.h);
    SDL_RenderCopy(renderer, global_ball.image_path, 0, &ball_rect);

    debug_draw_fps(dt);
}

#endif
