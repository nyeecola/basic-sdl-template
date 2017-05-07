#include "game.hpp"

particle_t spawn_particle_towards(v2 pos, v2 vector, double speed, v2 acceleration, const char *image, int w, int h, v3 color)
{
    assert(image);

    v2 direction;
    if (!is_null_vector(vector)) direction = math_normalize(vector);
    else direction = vector;

    // TODO: maybe use malloc, since this is called so many times
    //       if so, remember to free
    particle_t particle = {};
    particle.pos = pos;
    particle.velocity = direction * speed;
    particle.acceleration = acceleration;
    particle.image_path = image;
    particle.w = w;
    particle.h = h;
    particle.color = color;

    return particle;
}

void update_particle_position(particle_t *particle, double dt)
{
    // TODO: think how to do this (change accelearation) (probably using pointer to function?)
    //v2 ortho = V2(-particle->velocity.y, particle->velocity.x) * 0.0005;
    //particle->acceleration = rotate_vector(ortho, -0.1f);

    particle->velocity += particle->acceleration;
    particle->pos += particle->velocity * dt;
}

// TODO: make section of circle a parameter
inline void do_circular_atk(std::list<particle_t> *particles, atk_pattern_t *atk, double dt)
{
    // spawn at the given rate
    if (atk->time_since_last_spawn > atk->spawn_rate)
    {
        atk->time_since_last_spawn = 0;

        double x, y;
        //for (int i = 0; i < atk->particles_per_spawn; i++)
        for (int i = 0; i < atk->particles_per_spawn; i++)
        {
            int step = atk->rotation_type == ROTATE_CW ? 1 : -1;

            double radians = ((atk->arc_size / atk->particles_per_spawn) * i * step) +
                             (atk->last_angle += atk->angle_step * step) +
                             atk->arc_center - atk->arc_size / 2;
            x = cos(radians);
            y = sin(radians);

            // TODO: consider what to do with this
            //v2 accel = V2(-y, x) * 0.0001;

            particle_t particle = spawn_particle_towards(*atk->spawn_loc,
                                                         V2(x, y),
                                                         atk->particle_speed,
                                                         V2(0, 0),
                                                         atk->particle_image,
                                                         atk->particle_width,
                                                         atk->particle_height,
                                                         atk->particle_color);
            particles->push_back(particle);
        }
    }
    else
    {
        atk->time_since_last_spawn += dt;
    }
}

// all ai actions are here
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

    // TODO: maybe consider movement an action? or something different?
    //       (find a way to only move from time to time)
    //       (also, it should be possible to synchronize movement with attacks)
    // movement
    {
        // if stopped, acquire new target location
        if (!enemy->enemy_data.forced_movement && enemy->enemy_data.stopped)
        {
            int new_target = rand() % enemy->enemy_data.path_size;
            enemy->enemy_data.target_loc = enemy->enemy_data.path[new_target];
            enemy->enemy_data.stopped = false;
        }

        // move to current target location
        v2 offset = enemy->enemy_data.target_loc - enemy->pos;
        double magnitude = math_magnitude(offset);
        v2 direction;
        if (!is_null_vector(offset)) direction = math_normalize(offset);
        else direction = offset;
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

    // attacks
    {
        if (enemy->enemy_data.time_since_fight_started > 5 &&
                enemy->enemy_data.time_since_fight_started < 8)
        {
            do_circular_atk(game->particles, &enemy->enemy_data.atks[0], dt);
        }

        if (enemy->enemy_data.time_since_fight_started > 2 &&
                enemy->enemy_data.time_since_fight_started < 9)
        {
            do_circular_atk(game->particles, &enemy->enemy_data.atks[1], dt);
            do_circular_atk(game->particles, &enemy->enemy_data.atks[2], dt);
        }

        if (enemy->enemy_data.time_since_fight_started > 10 &&
            enemy->enemy_data.time_since_fight_started < 20)
        {
            if (!enemy->enemy_data.forced_movement)
            {
                enemy->enemy_data.forced_movement = true;
                enemy->enemy_data.stopped = false;

                v2 loc = V2(300, 100);
                enemy->enemy_data.target_loc = loc;
            }

            if (enemy->enemy_data.stopped)
            {
                // TODO: verify if null vector
                v2 player_dir = game->keybd_ball.pos - enemy->pos;
                if (is_null_vector(player_dir))
                {
                    player_dir = math_normalize(player_dir);
                }
                double angle = atan2(player_dir.y, player_dir.x);
                enemy->enemy_data.atks[3].arc_center = angle;
                do_circular_atk(game->particles, &enemy->enemy_data.atks[3], dt);
            }
        }

        if (enemy->enemy_data.time_since_fight_started > 20)
        {
            enemy->enemy_data.forced_movement = false;
        }
    }

    enemy->enemy_data.time_since_fight_started += dt;

    // TODO: stop attacking once the fight is over (maybe destroy enemy or reset timer to reuse?)
}

game_state_t *game_state_initialize()
{
    game_state_t *game = (game_state_t *) calloc(1, sizeof(*game));
    game->particles = new std::list<particle_t>();

    // background_color
    game->background_color.red = 0;
    game->background_color.green = 85;
    game->background_color.blue = 127;
    game->background_color.alpha = 255;

    // keyboard controlled ball
    game->keybd_ball = {};
    game->keybd_ball.image_path = BALL_IMG_PATH;
    game->keybd_ball.w = 30;
    game->keybd_ball.h = 42;
    game->keybd_ball.pos = V2(300, 700);
    game->keybd_ball.speed = 200;

    // keyboard controlled ball
    game->mouse_ball = {};
    game->mouse_ball.image_path = BALL_IMG_PATH;
    game->mouse_ball.w = 100;
    game->mouse_ball.h = 100;

    // enemy
    {
        game->enemy = {};
        game->enemy.type = ENTITY_ENEMY;
        game->enemy.image_path = BALL_IMG_PATH;
        game->enemy.w = 100;
        game->enemy.h = 100;
        game->enemy.pos = V2(400, 60);
        game->enemy.speed = 60;
        game->enemy.enemy_data.time_since_fight_started = 0.0;

        // movement
        {
            game->enemy.enemy_data.stopped = true;
            game->enemy.enemy_data.path[0] = V2(150, 150);
            game->enemy.enemy_data.path[1] = V2(500, 100);
            game->enemy.enemy_data.path[2] = V2(280, 200);
            game->enemy.enemy_data.path[3] = V2(30, 80);
            game->enemy.enemy_data.path[4] = V2(300, 120);
            game->enemy.enemy_data.path[5] = V2(530, 180);
            game->enemy.enemy_data.path_size = 6;
        }

        // attacks
        {
            // fast spiral from enemy
            {
                atk_pattern_t atk = {};
                atk.particle_image = BALL_IMG_PATH;
                atk.particle_width = 20;
                atk.particle_height = 20;
                atk.particle_color = V3(0, 0, 0);
                atk.spawn_loc = &game->enemy.pos;
                atk.spawn_rate = 0.20;
                atk.particles_per_spawn = 20;
                atk.arc_center = 0;
                atk.arc_size = PI * 2;
                atk.angle_step = 0.0;
                atk.last_angle = 0.0;
                atk.particle_speed = 250;
                game->enemy.enemy_data.atks[0] = atk;
            }

            // two medium-to-fast spirals from the top corners of the screen
            {
                atk_pattern_t atk = {};

                atk.particle_image = BALL_IMG_PATH;
                atk.particle_width = 30;
                atk.particle_height = 30;
                atk.spawn_rate = 0.25;
                atk.particles_per_spawn = 12;
                atk.arc_center = 0;
                atk.arc_size = PI * 2;
                atk.angle_step = 0.006;
                atk.last_angle = 0.0;
                atk.particle_speed = 150;

                atk.rotation_type = ROTATE_CW;
                atk.particle_color = V3(180, 180, 255);
                v2 *atk1_loc = (v2 *) malloc(sizeof(*atk1_loc));
                *atk1_loc = V2(50, 150);
                atk.spawn_loc = atk1_loc;
                game->enemy.enemy_data.atks[1] = atk;

                //atk.rotation_type = ROTATE_CCW;
                atk.particle_color = V3(255, 180, 180);
                v2 *atk2_loc = (v2 *) malloc(sizeof(*atk2_loc));
                *atk2_loc = V2(550, 150);
                atk.spawn_loc = atk2_loc;
                game->enemy.enemy_data.atks[2] = atk;
            }

            // dense small arc
            {
                atk_pattern_t atk = {};
                atk.particle_image = BALL_IMG_PATH;
                atk.particle_width = 16;
                atk.particle_height = 22;
                atk.particle_color = V3(0, 255, 0);
                atk.spawn_loc = &game->enemy.pos;
                atk.spawn_rate = 0.1;
                atk.particles_per_spawn = 8;
                atk.arc_center = PI / 2 + ((PI / 16) / atk.particles_per_spawn) / 2;
                atk.arc_size = PI / 16 + (PI / 16) / atk.particles_per_spawn;
                atk.angle_step = 0.0;
                atk.last_angle = 0.0;
                atk.particle_speed = 500;
                game->enemy.enemy_data.atks[3] = atk;
            }
        }
    }

    return game;
}

void game_state_update(game_state_t *game, input_t *input, double dt)
{
    assert(game);
    assert(input);
    assert(dt > 0);

    // keyboard ball movement
    {
        double speed = game->keybd_ball.speed;
        if (input->keys_pressed[SDL_SCANCODE_LSHIFT] || input->keys_pressed[SDL_SCANCODE_RSHIFT])
        {
            speed *= 0.65;
        }
        v2 offset_dir = {0, 0};
        if (input->keys_pressed[SDL_SCANCODE_UP])
        {
            offset_dir.y = -1;
        }
        if (input->keys_pressed[SDL_SCANCODE_DOWN])
        {
            offset_dir.y = 1;
        }
        if (input->keys_pressed[SDL_SCANCODE_LEFT])
        {
            offset_dir.x = -1;
        }
        if (input->keys_pressed[SDL_SCANCODE_RIGHT])
        {
            offset_dir.x = 1;
        }
        if (!is_null_vector(offset_dir))
        {
            offset_dir = math_normalize(offset_dir);
        }
        game->keybd_ball.pos += offset_dir * speed * dt;

        if (game->keybd_ball.pos.x < 0)
        {
            game->keybd_ball.pos.x = 0;
        }
        if (game->keybd_ball.pos.y < 0)
        {
            game->keybd_ball.pos.y = 0;
        }
        if (game->keybd_ball.pos.x > DEFAULT_SCREEN_WIDTH)
        {
            game->keybd_ball.pos.x = DEFAULT_SCREEN_WIDTH;
        }
        if (game->keybd_ball.pos.y > DEFAULT_SCREEN_HEIGHT)
        {
            game->keybd_ball.pos.y = DEFAULT_SCREEN_HEIGHT;
        }
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
        rect.x = (int) round(game->keybd_ball.pos.x - game->keybd_ball.w / 2);
        rect.y = (int) round(game->keybd_ball.pos.y - game->keybd_ball.h / 2);
        rect.w = (int) round(game->keybd_ball.w);
        rect.h = (int) round(game->keybd_ball.h);
        display_image(renderer, game->keybd_ball.image_path, &rect, V3(255, 255, 255));

        // mouse ball
        rect.x = (int) round(game->mouse_ball.pos.x - game->mouse_ball.w / 2);
        rect.y = (int) round(game->mouse_ball.pos.y - game->mouse_ball.h / 2);
        rect.w = (int) round(game->mouse_ball.w);
        rect.h = (int) round(game->mouse_ball.h);
        display_image(renderer, game->mouse_ball.image_path, &rect, V3(255, 255, 255));

        // enemy ball
        rect.x = (int) round(game->enemy.pos.x - game->enemy.w / 2);
        rect.y = (int) round(game->enemy.pos.y - game->enemy.h / 2);
        rect.w = (int) round(game->enemy.w);
        rect.h = (int) round(game->enemy.h);
        display_image(renderer, game->enemy.image_path, &rect, V3(255, 255, 255));
    }

    // render particles
    {
        SDL_Rect rect;
        std::list<particle_t>::iterator it, end;
        for (it = game->particles->begin(), end = game->particles->end(); it != end; ++it)
        {
            particle_t *particle = &(*it);

            rect.x = (int) round(particle->pos.x - particle->w / 2);
            rect.y = (int) round(particle->pos.y - particle->h / 2);
            rect.w = (int) round(particle->w);
            rect.h = (int) round(particle->h);

            display_image(renderer, particle->image_path, &rect, particle->color);
        }
    }
}
