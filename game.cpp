#include "game.hpp"

game_state_t *game_state_initialize(SDL_Renderer *renderer)
{
    game_state_t *game_state = (game_state_t *) malloc(sizeof(*game_state));

    // background_color
    game_state->background_color.red = 0;
    game_state->background_color.green = 0;
    game_state->background_color.blue = 127;
    game_state->background_color.alpha = 255;

    // keyboard controlled ball
    game_state->keybd_ball.image = IMG_LoadTexture(renderer, BALL_IMG_PATH);
    SDL_QueryTexture(game_state->keybd_ball.image, 0, 0, &game_state->keybd_ball.w, &game_state->keybd_ball.h);
    game_state->keybd_ball.x = 40;
    game_state->keybd_ball.y = 40;
    game_state->keybd_ball.speed = BALL_SPEED;

    // keyboard controlled ball
    game_state->mouse_ball.image = IMG_LoadTexture(renderer, BALL_IMG_PATH);
    SDL_QueryTexture(game_state->mouse_ball.image, 0, 0, &game_state->mouse_ball.w, &game_state->mouse_ball.h);
    game_state->mouse_ball.speed = BALL_SPEED; // FIXME: verify if needed

    // enemy ball
    game_state->enemy_ball.image = IMG_LoadTexture(renderer, BALL_IMG_PATH);
    SDL_QueryTexture(game_state->enemy_ball.image, 0, 0, &game_state->enemy_ball.w, &game_state->enemy_ball.h);
    game_state->enemy_ball.x = 400;
    game_state->enemy_ball.y = 60;
    game_state->enemy_ball.speed = BALL_SPEED;

    return game_state;
}

void game_state_update(game_state_t *game_state, input_t *input, double dt)
{
    // keyboard ball movement
    if (input->keys_pressed[SDL_SCANCODE_UP])
    {
        game_state->keybd_ball.y -= game_state->keybd_ball.speed * dt;
    }
    if (input->keys_pressed[SDL_SCANCODE_DOWN])
    {
        game_state->keybd_ball.y += game_state->keybd_ball.speed * dt;
    }
    if (input->keys_pressed[SDL_SCANCODE_LEFT])
    {
        game_state->keybd_ball.x -= game_state->keybd_ball.speed * dt;
    }
    if (input->keys_pressed[SDL_SCANCODE_RIGHT])
    {
        game_state->keybd_ball.x += game_state->keybd_ball.speed * dt;
    }

    // mouse ball movement
    int mouse_x, mouse_y;
    SDL_GetMouseState(&mouse_x, &mouse_y);
    game_state->mouse_ball.x = mouse_x;
    game_state->mouse_ball.y = mouse_y;

    // enemy ball movement
    int boundary_left = 200;
    int boundary_right = 600;
    
    

    double multiplier = 0.1;
    static bool red_decreasing = false;
    if (red_decreasing)
    {
        game_state->background_color.red -= multiplier * dt;
        if (game_state->background_color.red < 5) red_decreasing = false;
    }
    else
    {
        game_state->background_color.red += multiplier * dt;
        if (game_state->background_color.red > 250) red_decreasing = true;
    }

    static bool blue_decreasing = false;
    if (blue_decreasing)
    {
        game_state->background_color.blue -= multiplier * dt;
        if (game_state->background_color.blue < 5) blue_decreasing = false;
    }
    else
    {
        game_state->background_color.blue += multiplier * dt;
        if (game_state->background_color.blue > 250) blue_decreasing = true;
    }
}

void game_state_render(game_state_t *game_state, SDL_Renderer *renderer, double)
{
    // repaint background
    SDL_SetRenderDrawColor(renderer,
                           (u8) round(game_state->background_color.red),
                           (u8) round(game_state->background_color.green),
                           (u8) round(game_state->background_color.blue),
                           (u8) round(game_state->background_color.alpha));
    SDL_RenderClear(renderer);

    // draw balls
    {
        SDL_Rect ball_rect;

        // keyboard ball
        ball_rect.x = round(game_state->keybd_ball.x - (BALL_SCALE * game_state->keybd_ball.w) / 2);
        ball_rect.y = round(game_state->keybd_ball.y - (BALL_SCALE * game_state->keybd_ball.h) / 2);
        ball_rect.w = round(BALL_SCALE * game_state->keybd_ball.w);
        ball_rect.h = round(BALL_SCALE * game_state->keybd_ball.h);
        SDL_RenderCopy(renderer, game_state->keybd_ball.image, 0, &ball_rect);

        // mouse ball
        ball_rect.x = round(game_state->mouse_ball.x - (BALL_SCALE * game_state->mouse_ball.w) / 2);
        ball_rect.y = round(game_state->mouse_ball.y - (BALL_SCALE * game_state->mouse_ball.h) / 2);
        ball_rect.w = round(BALL_SCALE * game_state->mouse_ball.w);
        ball_rect.h = round(BALL_SCALE * game_state->mouse_ball.h);
        SDL_RenderCopy(renderer, game_state->mouse_ball.image, 0, &ball_rect);

        // enemy ball
        ball_rect.x = round(game_state->enemy_ball.x - (BALL_SCALE * game_state->enemy_ball.w) / 2);
        ball_rect.y = round(game_state->enemy_ball.y - (BALL_SCALE * game_state->enemy_ball.h) / 2);
        ball_rect.w = round(BALL_SCALE * game_state->enemy_ball.w);
        ball_rect.h = round(BALL_SCALE * game_state->enemy_ball.h);
        SDL_RenderCopy(renderer, game_state->enemy_ball.image, 0, &ball_rect);
    }
}
