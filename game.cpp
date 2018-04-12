void enemy_move_around(entity_t *enemy, double dt) {
    assert(enemy->type == ENTITY_ENEMY);
    assert(dt > 0);

    // if stopped, acquire new target location
    if (enemy->enemy_data.stopped)    {
        int new_target = rand() % enemy->enemy_data.path_size;
        enemy->enemy_data.target_loc = enemy->enemy_data.path[new_target];
        enemy->enemy_data.stopped = false;
    }

    // move to current target location
    v2 offset = enemy->enemy_data.target_loc - enemy->pos;
    double magnitude = math_magnitude(offset);
    v2 direction = math_normalize(offset);
    v2 velocity = direction * enemy->speed * dt;
    if (magnitude < enemy->speed * dt) {
        enemy->pos += offset;
        enemy->enemy_data.stopped = true;
    } else {
        enemy->pos += velocity;
    }
}

game_state_t *game_state_initialize(SDL_Renderer *renderer) {
    assert(renderer);

    game_state_t *game_state = (game_state_t *) malloc(sizeof(*game_state));

    // background_color
    game_state->background_color.red = 0;
    game_state->background_color.green = 0;
    game_state->background_color.blue = 127;
    game_state->background_color.alpha = 255;

    // keyboard controlled ball
    game_state->keybd_ball = {};
    game_state->keybd_ball.image = IMG_LoadTexture(renderer, BALL_IMG_PATH);
    SDL_QueryTexture(game_state->keybd_ball.image, 0, 0, &game_state->keybd_ball.w, &game_state->keybd_ball.h);
    game_state->keybd_ball.pos = V2(40, 40);
    game_state->keybd_ball.speed = BALL_SPEED;

    // keyboard controlled ball
    game_state->mouse_ball = {};
    game_state->mouse_ball.image = IMG_LoadTexture(renderer, BALL_IMG_PATH);
    SDL_QueryTexture(game_state->mouse_ball.image, 0, 0, &game_state->mouse_ball.w, &game_state->mouse_ball.h);
    game_state->mouse_ball.speed = BALL_SPEED; // FIXME: verify if needed

    // enemy
    game_state->enemy = {};
    game_state->enemy.type = ENTITY_ENEMY;
    game_state->enemy.image = IMG_LoadTexture(renderer, BALL_IMG_PATH);
    SDL_QueryTexture(game_state->enemy.image, 0, 0, &game_state->enemy.w, &game_state->enemy.h);
    game_state->enemy.pos = V2(400, 60);
    game_state->enemy.speed = 0.4;
    game_state->enemy.enemy_data.stopped = true;
    game_state->enemy.enemy_data.path[0] = V2(150, 250);
    game_state->enemy.enemy_data.path[1] = V2(600, 400);
    game_state->enemy.enemy_data.path[2] = V2(380, 530);
    game_state->enemy.enemy_data.path[3] = V2(30, 80);
    game_state->enemy.enemy_data.path[4] = V2(300, 120);
    game_state->enemy.enemy_data.path[5] = V2(700, 130);
    game_state->enemy.enemy_data.path_size = 6;

    return game_state;
}

void game_state_update(game_state_t *game_state, input_t *input, double dt) {
    assert(game_state);
    assert(input);
    assert(dt > 0);

    // keyboard ball movement
    if (input->keys_pressed[SDL_SCANCODE_UP]) {
        game_state->keybd_ball.pos.y -= game_state->keybd_ball.speed * dt;
    }
    if (input->keys_pressed[SDL_SCANCODE_DOWN]) {
        game_state->keybd_ball.pos.y += game_state->keybd_ball.speed * dt;
    }
    if (input->keys_pressed[SDL_SCANCODE_LEFT]) {
        game_state->keybd_ball.pos.x -= game_state->keybd_ball.speed * dt;
    }
    if (input->keys_pressed[SDL_SCANCODE_RIGHT]) {
        game_state->keybd_ball.pos.x += game_state->keybd_ball.speed * dt;
    }

    // mouse ball movement
    int mouse_x, mouse_y;
    SDL_GetMouseState(&mouse_x, &mouse_y);
    game_state->mouse_ball.pos.x = mouse_x;
    game_state->mouse_ball.pos.y = mouse_y;

    // enemy movement
    enemy_move_around(&game_state->enemy, dt);

    // update background
    {
        double multiplier = 0.1;
        static bool red_decreasing = false;
        if (red_decreasing) {
            game_state->background_color.red -= multiplier * dt;
            if (game_state->background_color.red < 5) red_decreasing = false;
        } else {
            game_state->background_color.red += multiplier * dt;
            if (game_state->background_color.red > 250) red_decreasing = true;
        }
        static bool blue_decreasing = false;
        if (blue_decreasing) {
            game_state->background_color.blue -= multiplier * dt;
            if (game_state->background_color.blue < 5) blue_decreasing = false;
        } else {
            game_state->background_color.blue += multiplier * dt;
            if (game_state->background_color.blue > 250) blue_decreasing = true;
        }
    }
}

void game_state_render(game_state_t *game_state, SDL_Renderer *renderer, double) {
    assert(game_state);
    assert(renderer);

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
        ball_rect.x = round(game_state->keybd_ball.pos.x - (BALL_SCALE * game_state->keybd_ball.w) / 2);
        ball_rect.y = round(game_state->keybd_ball.pos.y - (BALL_SCALE * game_state->keybd_ball.h) / 2);
        ball_rect.w = round(BALL_SCALE * game_state->keybd_ball.w);
        ball_rect.h = round(BALL_SCALE * game_state->keybd_ball.h);
        SDL_RenderCopy(renderer, game_state->keybd_ball.image, 0, &ball_rect);

        // mouse ball
        ball_rect.x = round(game_state->mouse_ball.pos.x - (BALL_SCALE * game_state->mouse_ball.w) / 2);
        ball_rect.y = round(game_state->mouse_ball.pos.y - (BALL_SCALE * game_state->mouse_ball.h) / 2);
        ball_rect.w = round(BALL_SCALE * game_state->mouse_ball.w);
        ball_rect.h = round(BALL_SCALE * game_state->mouse_ball.h);
        SDL_RenderCopy(renderer, game_state->mouse_ball.image, 0, &ball_rect);

        // enemy ball
        ball_rect.x = round(game_state->enemy.pos.x - (BALL_SCALE * game_state->enemy.w) / 2);
        ball_rect.y = round(game_state->enemy.pos.y - (BALL_SCALE * game_state->enemy.h) / 2);
        ball_rect.w = round(BALL_SCALE * game_state->enemy.w);
        ball_rect.h = round(BALL_SCALE * game_state->enemy.h);
        SDL_RenderCopy(renderer, game_state->enemy.image, 0, &ball_rect);
    }
}
