#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <cstdio>
#include <cstring>

#include "utils.hpp"

const int FONT_SIZE = 32;

static TTF_Font *global_font = 0;
static SDL_Renderer *global_renderer = 0;
static SDL_Texture *global_glyphs[100] = {};

void create_texture_for_glyphs(const char *str, int index)
{
    // TODO: parametrize font color
    SDL_Color white;
    white.r = 255;
    white.g = 255;
    white.b = 255;
    white.a = 255;

    // TODO: error handling
    SDL_Surface *surface = TTF_RenderText_Solid(global_font, str, white);

    // TODO: error handling
    SDL_Texture *texture = SDL_CreateTextureFromSurface(global_renderer, surface);

    global_glyphs[index] = texture;

    // TODO: error handling (could be a nullptr)
    SDL_FreeSurface(surface);
}

void debug_initialize_text(SDL_Renderer *renderer)
{
    if(TTF_Init() == -1) {
        puts("Failed to initialize TTF.");
        printf("Error: %s\n", TTF_GetError());
        exit(1);
    }

    global_renderer = renderer;

    // TODO: handle error
    global_font = TTF_OpenFont("assets/Inconsolata-Regular.ttf", FONT_SIZE);

    // create a texture for each glyph
    create_texture_for_glyphs("0", '0');
    create_texture_for_glyphs("1", '1');
    create_texture_for_glyphs("2", '2');
    create_texture_for_glyphs("3", '3');
    create_texture_for_glyphs("4", '4');
    create_texture_for_glyphs("5", '5');
    create_texture_for_glyphs("6", '6');
    create_texture_for_glyphs("7", '7');
    create_texture_for_glyphs("8", '8');
    create_texture_for_glyphs("9", '9');
}

// NOTE: this is too inefficient, do not call this every frame
// FIXME: remove this once the fast version is finished, also be aware that
//        this function has no error handling
void debug_slow_draw_text(const char *str)
{
    SDL_Color white;
    white.r = 255;
    white.g = 255;
    white.b = 255;
    white.a = 255;

    SDL_Surface *surface = TTF_RenderText_Solid(global_font, str, white);

    SDL_Texture *texture = SDL_CreateTextureFromSurface(global_renderer, surface);

    SDL_Rect rect;
    rect.x = 50;
    rect.y = 50;
    SDL_QueryTexture(texture, 0, 0, &rect.w, &rect.h);

    SDL_RenderCopy(global_renderer, texture, 0, &rect);

    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

void debug_fast_draw_text(const char *str, int x, int y)
{
    int len = strlen(str);
    for (int i = 0; i < len; i++)
    {
        SDL_Texture *glyph = global_glyphs[(int) str[i]];
        SDL_Rect rect;

        if(SDL_QueryTexture(glyph, 0, 0, &rect.w, &rect.h) < 0)
        {
            printf("Error: %s\n", SDL_GetError());
            exit(1);
        }

        rect.x = x + rect.w * i;
        rect.y = y;

        if(SDL_RenderCopy(global_renderer, glyph, 0, &rect) < 0)
        {
            printf("Error: %s\n", SDL_GetError());
            exit(1);
        }
    }
}

// FIXME: rework this
void debug_draw_fps(double dt)
{
    static double timer = 0;
    static char fps_text[15] = "";
    timer += dt;
    if (timer > 20) 
    {
        snprintf(&fps_text[0], 15, "%.00f", 1 / (dt / 1000));
        timer = 0;
    }
    debug_fast_draw_text(&fps_text[0], 0, 0);
}

#endif
