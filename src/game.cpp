game_state_t *game_state_initialize(SDL_Renderer *renderer) {
    assert(renderer);

    game_state_t *game_state = (game_state_t *) malloc(sizeof(*game_state));

    // game mode
    game_state->game_mode = PLAYING;

    // background_color
    game_state->background_color.r = 0;
    game_state->background_color.g = 0;
    game_state->background_color.b = 127;
    game_state->background_color.a = 255;

    // initialize map grid (currently only for testing)
    game_state->map.w = 40;
    game_state->map.h = 30;
    for (int i = 0; i < game_state->map.w; i++) {
        for (int j = 0; j < game_state->map.h; j++) {
            if (!i || !j || i == game_state->map.w-1 || j == game_state->map.h-1) {
                game_state->map.tile[j][i] = WALL;
            } else {
                game_state->map.tile[j][i] = EMPTY;
            }
        }
    }
    game_state->map.wall_sprite = IMG_LoadTexture(renderer, WALL_IMG_PATH);
    assert(game_state->map.wall_sprite);
    game_state->map.floor_sprite = IMG_LoadTexture(renderer, FLOOR_IMG_PATH);
    assert(game_state->map.floor_sprite);

    return game_state;
}

void game_state_update(game_state_t *game_state, input_t *input, double dt) {
    assert(game_state);
    assert(input);
    assert(dt >= 0);

    switch(game_state->game_mode) {
        case PLAYING:
            // keyboard ball movement
            if (input->keys_pressed[SDL_SCANCODE_UP]) {
            }
            if (input->keys_pressed[SDL_SCANCODE_DOWN]) {
            }
            if (input->keys_pressed[SDL_SCANCODE_LEFT]) {
            }
            if (input->keys_pressed[SDL_SCANCODE_RIGHT]) {
            }
            break;
        case PAUSED:
            break;
        case START_SCREEN:
            break;
    }
}

// temporary
void render_ball(SDL_Renderer *renderer, entity_t ball) {
    assert(renderer);

    SDL_Rect ball_rect;
    ball_rect.x = round(ball.pos.x - (BALL_SCALE * ball.w) / 2);
    ball_rect.y = round(ball.pos.y - (BALL_SCALE * ball.h) / 2);
    ball_rect.w = round(BALL_SCALE * ball.w);
    ball_rect.h = round(BALL_SCALE * ball.h);
    SDL_RenderCopy(renderer, ball.image, 0, &ball_rect);
}

void game_state_render(game_state_t *game_state, SDL_Renderer *renderer, double dt) {
    assert(game_state);
    assert(renderer);

    map_t m;
    memcpy(&m, &game_state->map, sizeof(m));
    switch(game_state->game_mode) {
        case PLAYING:
            // repaint background
            SDL_RenderClear(renderer);
            for (int i = 0; i < m.w; i++) {
                for (int j = 0; j < m.h; j++) {
                    SDL_Rect rect;
                    rect.x = 20 * i;
                    rect.y = 20 * j;
                    rect.w = 20;
                    rect.h = 20;
                    switch(m.tile[j][i]) {
                        case EMPTY:
                            SDL_RenderCopy(renderer, m.floor_sprite, 0, &rect);
                            break;
                        case WALL:
                            SDL_RenderCopy(renderer, m.wall_sprite, 0, &rect);
                            break;
                        default:
                            assert(false);
                            break;
                    }
                }
            }
            break;
        case PAUSED:
            // repaint background as black
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 1);
            SDL_RenderClear(renderer);
            break;
        case START_SCREEN:
            break;
    }
}
