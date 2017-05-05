#include "game.hpp"

// TODO: delete particles after they've been offscreen for a reasonable amount of time
//       REMEMBER TO DECREMENT NUM_PARTICLES THEN
particle_t spawn_particle_towards(v2 pos, v2 vector, double speed, v2 acceleration, SDL_Texture *image)
{
    assert(image);

    v2 direction = math_normalize(vector);

    particle_t particle = {};
    particle.pos = pos;
    particle.velocity = direction * speed;
    particle.acceleration = acceleration;

    // FIXME: probably should find a better way to discover size and image
    particle.image = image;
    SDL_QueryTexture(particle.image, 0, 0, &particle.w, &particle.h);
    particle.w *= PARTICLE_SIZE;
    particle.h *= PARTICLE_SIZE;

    return particle;
}

void update_particle_position(particle_t *particle)
{
    particle->pos += particle->velocity;
}

void ai_do_actions(game_state_t *game, SDL_Renderer *renderer, double dt)
{
    assert(game);
    assert(dt > 0);

    // TODO: different behaviors
    //       probably each enemy should have different movement patterns (with a few exceptions)
    //       also, each enemy should have different attacks (maybe some few exceptions as well)

    // TODO: change this for a loop iterating over all ai controlled entities
    entity_t *enemy = &game->enemy;
    assert(enemy->type == ENTITY_ENEMY);
    // movement
    {
        // if stopped, acquire new target location
        if (enemy->enemy_data.stopped)    
        {
            int new_target = rand() % enemy->enemy_data.path_size;
            enemy->enemy_data.target_loc = enemy->enemy_data.path[new_target];
            enemy->enemy_data.stopped = false;
        }

        // move to current target location
        v2 offset = enemy->enemy_data.target_loc - enemy->pos;
        double magnitude = math_magnitude(offset);
        v2 direction = math_normalize(offset);
        v2 velocity = direction * enemy->speed * dt;
        if (magnitude < enemy->speed * dt)
        {
            enemy->pos += offset;
            enemy->enemy_data.stopped = true;
        }
        else
        {
            enemy->pos += velocity;
        }
    }

    // if not on cooldown, create particles
    if (enemy->enemy_data.time_since_last_action > enemy->enemy_data.cooldown) 
    {
        enemy->enemy_data.time_since_last_action = 0;

        SDL_Texture *image = IMG_LoadTexture(renderer, BALL_IMG_PATH);
        assert(image);

        game->particles[game->num_particles++] = spawn_particle_towards(enemy->pos, V2(2, 6), 0.4 * dt, V2(0, 0), image);
        game->particles[game->num_particles++] = spawn_particle_towards(enemy->pos, V2(0, 2), 0.4 * dt, V2(0, 0), image);
        game->particles[game->num_particles++] = spawn_particle_towards(enemy->pos, V2(-2, 6), 0.4 * dt, V2(0, 0), image);
    }
    else
    {
        enemy->enemy_data.time_since_last_action += dt;
    }
}

game_state_t *game_state_initialize(SDL_Renderer *renderer)
{
    assert(renderer);

    game_state_t *game = (game_state_t *) calloc(1, sizeof(*game));

    // background_color
    game->background_color.red = 0;
    game->background_color.green = 0;
    game->background_color.blue = 127;
    game->background_color.alpha = 255;

    // keyboard controlled ball
    game->keybd_ball = {};
    game->keybd_ball.image = IMG_LoadTexture(renderer, BALL_IMG_PATH);
    assert(game->keybd_ball.image);
    SDL_QueryTexture(game->keybd_ball.image, 0, 0, &game->keybd_ball.w, &game->keybd_ball.h);
    game->keybd_ball.w *= BALL_SCALE;
    game->keybd_ball.h *= BALL_SCALE;
    game->keybd_ball.pos = V2(40, 40);
    game->keybd_ball.speed = BALL_SPEED;

    // keyboard controlled ball
    game->mouse_ball = {};
    game->mouse_ball.image = IMG_LoadTexture(renderer, BALL_IMG_PATH);
    assert(game->mouse_ball.image);
    SDL_QueryTexture(game->mouse_ball.image, 0, 0, &game->mouse_ball.w, &game->mouse_ball.h);
    game->mouse_ball.w *= BALL_SCALE;
    game->mouse_ball.h *= BALL_SCALE;
    game->mouse_ball.speed = BALL_SPEED; // FIXME: verify if needed

    // enemy
    game->enemy = {};
    game->enemy.type = ENTITY_ENEMY;
    game->enemy.image = IMG_LoadTexture(renderer, BALL_IMG_PATH);
    SDL_QueryTexture(game->enemy.image, 0, 0, &game->enemy.w, &game->enemy.h);
    game->enemy.w *= BALL_SCALE;
    game->enemy.h *= BALL_SCALE;
    game->enemy.pos = V2(400, 60);
    game->enemy.speed = 0.4;
    game->enemy.enemy_data.cooldown = 1000;
    game->enemy.enemy_data.stopped = true;
    game->enemy.enemy_data.path[0] = V2(150, 250);
    game->enemy.enemy_data.path[1] = V2(600, 400);
    game->enemy.enemy_data.path[2] = V2(380, 530);
    game->enemy.enemy_data.path[3] = V2(30, 80);
    game->enemy.enemy_data.path[4] = V2(300, 120);
    game->enemy.enemy_data.path[5] = V2(700, 130);
    game->enemy.enemy_data.path_size = 6;

    return game;
}

void game_state_update(game_state_t *game, input_t *input, SDL_Renderer *renderer, double dt)
{
    assert(game);
    assert(input);
    assert(dt > 0);

    // keyboard ball movement
    if (input->keys_pressed[SDL_SCANCODE_UP])
    {
        game->keybd_ball.pos.y -= game->keybd_ball.speed * dt;
    }
    if (input->keys_pressed[SDL_SCANCODE_DOWN])
    {
        game->keybd_ball.pos.y += game->keybd_ball.speed * dt;
    }
    if (input->keys_pressed[SDL_SCANCODE_LEFT])
    {
        game->keybd_ball.pos.x -= game->keybd_ball.speed * dt;
    }
    if (input->keys_pressed[SDL_SCANCODE_RIGHT])
    {
        game->keybd_ball.pos.x += game->keybd_ball.speed * dt;
    }

    // mouse ball movement
    int mouse_x, mouse_y;
    SDL_GetMouseState(&mouse_x, &mouse_y);
    game->mouse_ball.pos.x = mouse_x;
    game->mouse_ball.pos.y = mouse_y;

    // perform enemy's actions
    ai_do_actions(game, renderer, dt);

    // update particles
    for (int i = 0; i < game->num_particles; i++)
    {
        update_particle_position(&game->particles[i]);
    }

    // update background
    {
        double multiplier = 0.1;
        static bool red_decreasing = false;
        if (red_decreasing)
        {
            game->background_color.red -= multiplier * dt;
            if (game->background_color.red < 5) red_decreasing = false;
        }
        else
        {
            game->background_color.red += multiplier * dt;
            if (game->background_color.red > 250) red_decreasing = true;
        }
        static bool blue_decreasing = false;
        if (blue_decreasing)
        {
            game->background_color.blue -= multiplier * dt;
            if (game->background_color.blue < 5) blue_decreasing = false;
        }
        else
        {
            game->background_color.blue += multiplier * dt;
            if (game->background_color.blue > 250) blue_decreasing = true;
        }
    }
}

void game_state_render(game_state_t *game, SDL_Renderer *renderer, double)
{
    assert(game);
    assert(renderer);

    // repaint background
    SDL_SetRenderDrawColor(renderer,
                           (u8) round(game->background_color.red),
                           (u8) round(game->background_color.green),
                           (u8) round(game->background_color.blue),
                           (u8) round(game->background_color.alpha));
    SDL_RenderClear(renderer);

    // draw balls
    {
        SDL_Rect rect;

        // keyboard ball
        rect.x = round(game->keybd_ball.pos.x - (game->keybd_ball.w) / 2);
        rect.y = round(game->keybd_ball.pos.y - (game->keybd_ball.h) / 2);
        rect.w = round(game->keybd_ball.w);
        rect.h = round(game->keybd_ball.h);
        SDL_RenderCopy(renderer, game->keybd_ball.image, 0, &rect);

        // mouse ball
        rect.x = round(game->mouse_ball.pos.x - (game->mouse_ball.w) / 2);
        rect.y = round(game->mouse_ball.pos.y - (game->mouse_ball.h) / 2);
        rect.w = round(game->mouse_ball.w);
        rect.h = round(game->mouse_ball.h);
        SDL_RenderCopy(renderer, game->mouse_ball.image, 0, &rect);

        // enemy ball
        rect.x = round(game->enemy.pos.x - (game->enemy.w) / 2);
        rect.y = round(game->enemy.pos.y - (game->enemy.h) / 2);
        rect.w = round(game->enemy.w);
        rect.h = round(game->enemy.h);
        SDL_RenderCopy(renderer, game->enemy.image, 0, &rect);
    }

    // render particles
    {
        SDL_Rect rect;
        for (int i = 0; i < game->num_particles; i++)
        {
            particle_t *particle = &game->particles[i];

            rect.x = round(particle->pos.x - (particle->w) / 2);
            rect.y = round(particle->pos.y - (particle->h) / 2);
            rect.w = round(particle->w);
            rect.h = round(particle->h);
            SDL_RenderCopy(renderer, particle->image, 0, &rect);
        }
    }
}
