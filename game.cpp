#include "game.hpp"

particle_t spawn_particle_towards(v2 pos, v2 vector, double speed, v2 acceleration, const char *image)
{
    assert(image);

    v2 direction = math_normalize(vector);

    particle_t particle = {};
    particle.pos = pos;
    particle.velocity = direction * speed;
    particle.acceleration = acceleration;
    particle.image_path = image;
    particle.w = 30;
    particle.h = 30;

    return particle;
}

void update_particle_position(particle_t *particle, double dt)
{
    // TODO: think how to do this
    //v2 ortho = V2(-particle->velocity.y, particle->velocity.x) * 0.0005;
    //particle->acceleration = rotate_vector(ortho, -0.1f);

    particle->velocity += particle->acceleration;
    particle->pos += particle->velocity * dt;
}

void ai_do_actions(game_state_t *game, renderer_t *renderer, double dt)
{
    assert(game);
    assert(renderer);
    assert(renderer->sdl);
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

        double x, y;
        int num_particles_to_gen = 16;
        for (int i = 0; i < num_particles_to_gen; i++)
        {
            double radians = (((2 * PI) / num_particles_to_gen) * i) + (enemy->enemy_data.last_angle += enemy->enemy_data.angle_step);
            x = cos(radians);
            y = sin(radians);

            double speed = 0.1;

            // TODO: think if I should make a function for this
            //v2 accel = V2(-y, x) * 0.0001; // not being used in this test

            game->particles->push_back(spawn_particle_towards(enemy->pos, V2(x, y), speed, V2(0, 0), BALL_IMG_PATH));
            //game->particles->push_back(spawn_particle_towards(V2(400, 300), V2(x, y), speed, V2(0, 0), BALL_IMG_PATH));
        }
    }
    else
    {
        enemy->enemy_data.time_since_last_action += dt;
    }
}

game_state_t *game_state_initialize(renderer_t *renderer)
{
    assert(renderer);
    assert(renderer->sdl);

    game_state_t *game = (game_state_t *) calloc(1, sizeof(*game));
    game->particles = new std::list<particle_t>();
    game->particles->clear(); // FIXME: not sure if needed

    // background_color
    game->background_color.red = 0;
    game->background_color.green = 0;
    game->background_color.blue = 127;
    game->background_color.alpha = 255;

    // keyboard controlled ball
    game->keybd_ball = {};
    game->keybd_ball.image = IMG_LoadTexture(renderer->sdl, BALL_IMG_PATH);
    assert(game->keybd_ball.image);
    SDL_QueryTexture(game->keybd_ball.image, 0, 0, &game->keybd_ball.w, &game->keybd_ball.h);
    game->keybd_ball.w *= BALL_SCALE;
    game->keybd_ball.h *= BALL_SCALE;
    game->keybd_ball.pos = V2(40, 40);
    game->keybd_ball.speed = BALL_SPEED;

    // keyboard controlled ball
    game->mouse_ball = {};
    game->mouse_ball.image = IMG_LoadTexture(renderer->sdl, BALL_IMG_PATH);
    assert(game->mouse_ball.image);
    SDL_QueryTexture(game->mouse_ball.image, 0, 0, &game->mouse_ball.w, &game->mouse_ball.h);
    game->mouse_ball.w *= BALL_SCALE;
    game->mouse_ball.h *= BALL_SCALE;
    game->mouse_ball.speed = BALL_SPEED; // FIXME: verify if needed

    // enemy
    game->enemy = {};
    game->enemy.type = ENTITY_ENEMY;
    game->enemy.image = IMG_LoadTexture(renderer->sdl, BALL_IMG_PATH);
    SDL_QueryTexture(game->enemy.image, 0, 0, &game->enemy.w, &game->enemy.h);
    game->enemy.w *= BALL_SCALE;
    game->enemy.h *= BALL_SCALE;
    game->enemy.pos = V2(400, 60);
    game->enemy.speed = 0.4;
    game->enemy.enemy_data.cooldown = 200;
    game->enemy.enemy_data.stopped = true;
    game->enemy.enemy_data.angle_step = 0.004f;
    game->enemy.enemy_data.last_angle = 0.0f;
    game->enemy.enemy_data.path[0] = V2(150, 250);
    game->enemy.enemy_data.path[1] = V2(600, 400);
    game->enemy.enemy_data.path[2] = V2(380, 530);
    game->enemy.enemy_data.path[3] = V2(30, 80);
    game->enemy.enemy_data.path[4] = V2(300, 120);
    game->enemy.enemy_data.path[5] = V2(700, 130);
    game->enemy.enemy_data.path_size = 6;

    return game;
}

void game_state_update(game_state_t *game, input_t *input, renderer_t *renderer, double dt)
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
    std::list<particle_t>::iterator it, end;
    for (it = game->particles->begin(), end = game->particles->end(); it != end;)
    {
        particle_t *particle = &(*it);

        update_particle_position(particle, dt);

        // TODO: use current instead of default (we don't have current yet :()
        if (particle->pos.x < -DEFAULT_SCREEN_WIDTH / 2 ||
            particle->pos.x > DEFAULT_SCREEN_WIDTH + DEFAULT_SCREEN_WIDTH / 2 ||
            particle->pos.y < -DEFAULT_SCREEN_HEIGHT ||
            particle->pos.y > DEFAULT_SCREEN_HEIGHT + DEFAULT_SCREEN_HEIGHT / 2)
        {
            it = game->particles->erase(it);
            continue;
        }

        ++it;
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

void game_state_render(game_state_t *game, renderer_t *renderer, double)
{
    assert(game);
    assert(renderer);
    assert(renderer->sdl);

    // repaint background
    SDL_SetRenderDrawColor(renderer->sdl,
                           (u8) round(game->background_color.red),
                           (u8) round(game->background_color.green),
                           (u8) round(game->background_color.blue),
                           (u8) round(game->background_color.alpha));
    SDL_RenderClear(renderer->sdl);

    // draw balls
    {
        SDL_Rect rect;

        // keyboard ball
        rect.x = round(game->keybd_ball.pos.x - (game->keybd_ball.w) / 2);
        rect.y = round(game->keybd_ball.pos.y - (game->keybd_ball.h) / 2);
        rect.w = round(game->keybd_ball.w);
        rect.h = round(game->keybd_ball.h);
        SDL_RenderCopy(renderer->sdl, game->keybd_ball.image, 0, &rect);

        // mouse ball
        rect.x = round(game->mouse_ball.pos.x - (game->mouse_ball.w) / 2);
        rect.y = round(game->mouse_ball.pos.y - (game->mouse_ball.h) / 2);
        rect.w = round(game->mouse_ball.w);
        rect.h = round(game->mouse_ball.h);
        SDL_RenderCopy(renderer->sdl, game->mouse_ball.image, 0, &rect);

        // enemy ball
        rect.x = round(game->enemy.pos.x - (game->enemy.w) / 2);
        rect.y = round(game->enemy.pos.y - (game->enemy.h) / 2);
        rect.w = round(game->enemy.w);
        rect.h = round(game->enemy.h);
        SDL_RenderCopy(renderer->sdl, game->enemy.image, 0, &rect);
    }

    // render particles
    {
        SDL_Rect rect;
        std::list<particle_t>::iterator it, end;
        for (it = game->particles->begin(), end = game->particles->end(); it != end; ++it)
        {
            particle_t *particle = &(*it);

            // render image if it wasn't rendered before
            std::string path(particle->image_path);
            if (!renderer->images.count(path))
            {
                SDL_Texture *tex = IMG_LoadTexture(renderer->sdl, BALL_IMG_PATH);
                renderer->images[path] = tex;
            }

            rect.x = round(particle->pos.x - (particle->w) / 2);
            rect.y = round(particle->pos.y - (particle->h) / 2);
            rect.w = round(particle->w);
            rect.h = round(particle->h);
            SDL_RenderCopy(renderer->sdl, renderer->images[path], 0, &rect);
        }
    }
}
