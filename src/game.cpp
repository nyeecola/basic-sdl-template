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

    // initialize enemy
    game_state->enemies_count = 1;
    game_state->enemies[0].pos = V2(20,20);
    game_state->enemies[0].speed = 50;
    game_state->enemies[0].image = IMG_LoadTexture(renderer, BALL_IMG_PATH);
    game_state->enemies[0].image_w = 36;
    game_state->enemies[0].image_h = 36;
    game_state->enemies[0].type = ENEMY;

    // initialize map (currently only for testing)
    game_state->map = (map_t*) malloc(sizeof(*(game_state->map)) * MAX_DOOR_PER_ROOM);
    game_state->map[0].w = 40;
    game_state->map[0].h = 30;
    game_state->map[0].tile_size = 20;
    for (int i = 0; i < game_state->map[0].w; i++) {
        for (int j = 0; j < game_state->map[0].h; j++) {
            if (!i || !j || i == game_state->map[0].w-1 || j == game_state->map[0].h-1) {
                if ( (i == 0 && j == 20) || (i == 30 && j == game_state->map[0].h-1) ) {
                    game_state->map[0].tile[j][i] = DOOR;
                } else {
                    game_state->map[0].tile[j][i] = WALL;
                }
            }
        }
    }
    game_state->map[0].doors = 2;
    game_state->map[0].door[0].x = 0;
    game_state->map[0].door[0].y = 20;
    game_state->map[0].door[0].target_map = 0;
    game_state->map[0].door[0].target_door = 1;
    game_state->map[0].door[1].x = 30;
    game_state->map[0].door[1].y = game_state->map[0].h-1;
    game_state->map[0].door[1].target_map = 0;
    game_state->map[0].door[1].target_door = 0;


    game_state->current_map_id = 0;
    game_state->map[game_state->current_map_id].wall_sprite = IMG_LoadTexture(renderer, WALL_IMG_PATH);
    assert(game_state->map[game_state->current_map_id].wall_sprite);
    game_state->map[game_state->current_map_id].floor_sprite = IMG_LoadTexture(renderer, FLOOR_IMG_PATH);
    assert(game_state->map[game_state->current_map_id].floor_sprite);

    // initialize player data
    game_state->player.pos = V2(50, 50);
    game_state->player.speed = 150;
    game_state->player.image = IMG_LoadTexture(renderer, BALL_IMG_PATH);
    //SDL_QueryTexture(game_state->player.image, 0, 0,
                     //&game_state->player.image_w, &game_state->player.image_h);
    game_state->player.image_w = 36;
    game_state->player.image_h = 36;
    game_state->player.type = PLAYER;

    return game_state;
}


v2 entity_tile_pos(entity_t entity, map_t map) {
    int x, y;

    x = (int) entity.pos.x / map.tile_size;
    y = (int) entity.pos.y / map.tile_size;

    return V2((double)x,(double)y);
}

void handle_doors(game_state_t *game_state) {
    int x, y;

    map_t map = game_state->map[game_state->current_map_id];

    v2 pos = entity_tile_pos(game_state->player, map);

    x = (int) pos.x;
    y = (int) pos.y;

    assert(x >= 0);
    assert(y >= 0);
    assert(x < map.w);
    assert(y < map.h);


    if ( map.tile[y][x] == DOOR ) {
        for(int i=0 ; i < map.doors ; i++) {
            if ( map.door[i].x == x && map.door[i].y == y ) {
                game_state->current_map_id = map.door[i].target_map;
                map = game_state->map[game_state->current_map_id];
                for(int j=0 ; j < map.doors ; j++) {
                    if ( map.door[i].x == x && map.door[i].y == y ) {
                        // Player teleport;
                        printf("Entrou\n");
                    }
                }
            }
        }
    }
}


void game_state_update(game_state_t *game_state, input_t *input, double dt) {
    assert(game_state);
    assert(input);
    assert(dt >= 0);

    // fps counter
    //printf("dt: %lf\n", dt);

    entity_t *player = &game_state->player; 
    switch(game_state->game_mode) {
        case PLAYING:
            {
                // keyboard ball movement
                v2 velocity = V2(0,0);
                if (input->keys_pressed[SDL_SCANCODE_UP]) {
                    velocity += V2(0,-1);
                }
                if (input->keys_pressed[SDL_SCANCODE_DOWN]) {
                    velocity += V2(0,1);
                }
                if (input->keys_pressed[SDL_SCANCODE_LEFT]) {
                    velocity += V2(-1,0);
                }
                if (input->keys_pressed[SDL_SCANCODE_RIGHT]) {
                    velocity += V2(1,0);
                }
                if (math_magnitude(velocity)) {
                    player->pos += math_normalize(velocity) * player->speed * dt;
                }
            }
            handle_doors(game_state);
            break;
        case PAUSED:
            break;
        case START_SCREEN:
            break;
    }
}

void game_state_render(game_state_t *game_state, SDL_Renderer *renderer, double dt) {
    assert(game_state);
    assert(renderer);

    map_t m;
    switch(game_state->game_mode) {
        case PLAYING:
            // repaint background
            SDL_RenderClear(renderer);
            memcpy(&m, &game_state->map[game_state->current_map_id], sizeof(m));
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
                        case DOOR:
                            SDL_RenderCopy(renderer, m.floor_sprite, 0, &rect);
                            break;
                        default:
                            assert(false);
                            break;
                    }
                }
            }

            {
                int e_c = game_state->enemies_count;
                entity_t e[128];
                memcpy(&e, &game_state->enemies, sizeof(e));
                for (int i = 0; i < e_c; i++) {
                    SDL_Rect rect;
                    rect.x = e[i].pos.x - e[i].image_w/2;
                    rect.y = e[i].pos.y - e[i].image_h/2;
                    rect.w = e[i].image_w;
                    rect.h = e[i].image_h;
                    SDL_RenderCopy(renderer, e[i].image, 0, &rect);

                    enemy_goto(&game_state->enemies[i], V2(100,100), dt);
                }
            }

            // draw player
            {
                entity_t player = game_state->player;
                SDL_Rect rect;
                rect.x = player.pos.x - player.image_w/2;
                rect.y = player.pos.y - player.image_h/2;
                rect.w = player.image_w;
                rect.h = player.image_h;
                SDL_RenderCopy(renderer, player.image, 0, &rect);
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
