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

void ai_do_actions(game_state_t *game, double dt)
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
    if (enemy->enemy_data.time_since_last_action > enemy->enemy_data.spawn_rate) 
    {
        enemy->enemy_data.time_since_last_action = 0;

        double x, y;
        int num_particles_to_gen = 12;
        for (int i = 0; i < num_particles_to_gen; i++)
        {
            double radians = (((2 * PI) / num_particles_to_gen) * i) + (enemy->enemy_data.last_angle += enemy->enemy_data.angle_step);
            x = cos(radians);
            y = sin(radians);

            // TODO: think if I should make a function for this
            //v2 accel = V2(-y, x) * 0.0001; // not being used in this test

            //game->particles->push_back(spawn_particle_towards(enemy->pos, V2(x, y), enemy->enemy_data.particle_speed, V2(0, 0), BALL_IMG_PATH));
            game->particles->push_back(spawn_particle_towards(V2(400, 200), V2(x, y), enemy->enemy_data.particle_speed, V2(0, 0), BALL_IMG_PATH));
        }
    }
    else
    {
        enemy->enemy_data.time_since_last_action += dt;
    }
}

game_state_t *game_state_initialize()
{
    game_state_t *game = (game_state_t *) calloc(1, sizeof(*game));
    game->particles = new std::list<particle_t>();

    // background_color
    game->background_color.red = 0;
    game->background_color.green = 0;
    game->background_color.blue = 127;
    game->background_color.alpha = 255;

    // keyboard controlled ball
    game->keybd_ball = {};
    game->keybd_ball.image_path = BALL_IMG_PATH;
    game->keybd_ball.w = 40;
    game->keybd_ball.h = 70;
    game->keybd_ball.pos = V2(300, 700);
    game->keybd_ball.speed = 220;

    // keyboard controlled ball
    game->mouse_ball = {};
    game->mouse_ball.image_path = BALL_IMG_PATH;
    game->mouse_ball.w = 100;
    game->mouse_ball.h = 100;

    // enemy
    game->enemy = {};
    game->enemy.type = ENTITY_ENEMY;
    game->enemy.image_path = BALL_IMG_PATH;
    game->enemy.w = 100;
    game->enemy.h = 100;
    game->enemy.pos = V2(400, 60);
    game->enemy.speed = 60;
    game->enemy.enemy_data.cooldown = 3.0;
    game->enemy.enemy_data.spawn_rate = 0.08;
    game->enemy.enemy_data.stopped = true;
    game->enemy.enemy_data.angle_step = 0.01;
    game->enemy.enemy_data.last_angle = 0.0;
    game->enemy.enemy_data.particle_speed = 160;
    game->enemy.enemy_data.path[0] = V2(150, 150);
    game->enemy.enemy_data.path[1] = V2(500, 100);
    game->enemy.enemy_data.path[2] = V2(280, 200);
    game->enemy.enemy_data.path[3] = V2(30, 80);
    game->enemy.enemy_data.path[4] = V2(300, 120);
    game->enemy.enemy_data.path[5] = V2(530, 180);
    game->enemy.enemy_data.path_size = 6;

    return game;
}

void game_state_update(game_state_t *game, input_t *input, double dt)
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
    ai_do_actions(game, dt);

    // update particles
    std::list<particle_t>::iterator it, end;
    for (it = game->particles->begin(), end = game->particles->end(); it != end;)
    {
        particle_t *particle = &(*it);

        update_particle_position(particle, dt);

        // TODO: use current instead of default (we don't have current yet :()
        if (particle->pos.x < -DEFAULT_SCREEN_WIDTH / 3 ||
            particle->pos.x > DEFAULT_SCREEN_WIDTH + DEFAULT_SCREEN_WIDTH / 3 ||
            particle->pos.y < -DEFAULT_SCREEN_HEIGHT / 3 ||
            particle->pos.y > DEFAULT_SCREEN_HEIGHT + DEFAULT_SCREEN_HEIGHT / 3)
        {
            it = game->particles->erase(it);
            continue;
        }

        ++it;
    }

    // update background
    {
        double multiplier = 100;
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

void game_state_render(game_state_t *game, renderer_t *renderer)
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

    // render balls
    {
        SDL_Rect rect;

        // keyboard ball
        rect.x = (int) round(game->keybd_ball.pos.x - (game->keybd_ball.w) / 2);
        rect.y = (int) round(game->keybd_ball.pos.y - (game->keybd_ball.h) / 2);
        rect.w = (int) round(game->keybd_ball.w);
        rect.h = (int) round(game->keybd_ball.h);
        display_image(renderer, game->keybd_ball.image_path, &rect);

        // mouse ball
        rect.x = (int) round(game->mouse_ball.pos.x - (game->mouse_ball.w) / 2);
        rect.y = (int) round(game->mouse_ball.pos.y - (game->mouse_ball.h) / 2);
        rect.w = (int) round(game->mouse_ball.w);
        rect.h = (int) round(game->mouse_ball.h);
        display_image(renderer, game->mouse_ball.image_path, &rect);

        // enemy ball
        rect.x = (int) round(game->enemy.pos.x - (game->enemy.w) / 2);
        rect.y = (int) round(game->enemy.pos.y - (game->enemy.h) / 2);
        rect.w = (int) round(game->enemy.w);
        rect.h = (int) round(game->enemy.h);
        display_image(renderer, game->enemy.image_path, &rect);
    }

    // render particles
    {
        SDL_Rect rect;
        std::list<particle_t>::iterator it, end;
        for (it = game->particles->begin(), end = game->particles->end(); it != end; ++it)
        {
            particle_t *particle = &(*it);

            rect.x = (int) round(particle->pos.x - (particle->w) / 2);
            rect.y = (int) round(particle->pos.y - (particle->h) / 2);
            rect.w = (int) round(particle->w);
            rect.h = (int) round(particle->h);

            display_image(renderer, particle->image_path, &rect);
        }
    }
}
