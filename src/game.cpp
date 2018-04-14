void load_maps(map_t *map, SDL_Renderer *renderer) {

    FILE *arq = fopen(MAPS_SOURCE_PATH, "r");

    int maps;
    char current;

    fscanf(arq, "%d", &maps);
    assert(maps <= MAX_MAPS_PER_RUN);

    for(int m=0 ; m < maps ; m++) {
        fscanf(arq, "%d %d %d", &map[m].w, &map[m].h, &map[m].doors);
        do {
            fscanf(arq, "%c", &current);
        } while ( current != '\n');
        assert(map[m].w <= 40);
        assert(map[m].h <= 30);
        assert(map[m].doors <= MAX_DOOR_PER_ROOM);
        for(int y=0 ; y < map[m].h ; y++) {
            for(int x=0 ; x < map[m].w ; x++) {
                fscanf(arq, "%c", &current);
                switch(current) {
                    case ' ': {
                        map[m].tile[y][x] = EMPTY;
                        break;
                    }
                    case 'O': {
                        map[m].tile[y][x] = WALL;
                        break;
                    }
                    default: {
                        printf("%d\n", (int) current);
                        assert(false);
                        break;
                    }
                }
            }
            do {
                fscanf(arq, "%c", &current);
            } while ( current != '\n');
        }

        for(int i=0 ; i < map[m].doors ; i++) {
            fscanf(arq, "%d %d", &map[m].door[i].x, &map[m].door[i].y);
            fscanf(arq, "%d %d", &map[m].door[i].exit_x, &map[m].door[i].exit_y);
            fscanf(arq, "%d %d", &map[m].door[i].target_map, &map[m].door[i].target_door);
            map[m].tile[map[m].door[i].y][map[m].door[i].x] = DOOR;
        }
    }

    fclose(arq);

    map[0].door_sprite = IMG_LoadTexture(renderer, DOOR_IMG_PATH);
    assert(map[0].door_sprite);
    map[0].wall_sprite = IMG_LoadTexture(renderer, WALL_IMG_PATH);
    assert(map[0].wall_sprite);
    map[0].floor_sprite = IMG_LoadTexture(renderer, FLOOR_IMG_PATH);
    assert(map[0].floor_sprite);

    map[1].door_sprite = IMG_LoadTexture(renderer, DOOR_IMG_PATH);
    assert(map[1].door_sprite);
    map[1].wall_sprite = IMG_LoadTexture(renderer, WALL_IMG_PATH);
    assert(map[1].wall_sprite);
    map[1].floor_sprite = IMG_LoadTexture(renderer, FLOOR_IMG_PATH);
    assert(map[1].floor_sprite);
}


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
    game_state->enemies[0].image = IMG_LoadTexture(renderer, CAT_IMG_PATH);
    game_state->enemies[0].image_w = 36;
    game_state->enemies[0].image_h = 36;
    game_state->enemies[0].type = ENEMY;

    // initialize map (currently only for testing)
    game_state->map = (map_t*) malloc(sizeof(*(game_state->map)) * MAX_DOOR_PER_ROOM);
    game_state->current_map_id = 0;

    load_maps(game_state->map, renderer);

    // initialize player data
    game_state->player.pos = V2(50, 50);
    game_state->player.speed = 150;
    game_state->player.image = IMG_LoadTexture(renderer, CAT_IMG_PATH);
    //SDL_QueryTexture(game_state->player.image, 0, 0,
                     //&game_state->player.image_w, &game_state->player.image_h);
    game_state->player.image_w = 30;
    game_state->player.image_h = 30;
    game_state->player.type = PLAYER;
    game_state->player.hitbox_r = game_state->player.image_w/2;

    return game_state;
}

v2 entity_tile_pos(entity_t entity) {
    int x, y;

    x = (int) entity.pos.x / TILE_SIZE;
    y = (int) entity.pos.y / TILE_SIZE;

    return V2((double)x,(double)y);
}

void handle_doors(game_state_t *game_state) {
    int x, y;

    map_t map = game_state->map[game_state->current_map_id];

    v2 pos = entity_tile_pos(game_state->player);

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
                int d = map.door[i].target_door;
                map = game_state->map[game_state->current_map_id];
                door_t door = map.door[d];

                game_state->player.pos.x = door.exit_x * TILE_SIZE + TILE_SIZE/2;
                game_state->player.pos.y = door.exit_y * TILE_SIZE + TILE_SIZE/2;
            }
        }
    }
}

bool seg_intersects_circle(v2 c, double r, v2 a, v2 b) {
    assert(abs(a.x - b.x) >= 0.0001 || abs(a.y - b.y) >= 0.0001);

    v2 ab = math_normalize(b - a);
    v2 ac = c - a;

    double dp = ab * ac;

    double mag_ab = math_magnitude(b - a);

    v2 p;
    if (dp >= 0 && dp <= mag_ab) { // between a and b
        ab *= dp;
        p = ab + a;
    } else if (dp < 0) { // we have to check a
        p = a;
    } else { // we have to check b
        p = b;
    }

    return (math_magnitude(c-p) <= r);
}

// NOTE: clockwise points
bool rect_intersects_circle(v2 p, double r, v2 a, v2 b, v2 c, v2 d) {
    return (seg_intersects_circle(p, r, a, b) ||
            seg_intersects_circle(p, r, b, c) ||
            seg_intersects_circle(p, r, c, d) ||
            seg_intersects_circle(p, r, d, a));
}

bool collides_with_walls(v2 pos, double r, map_t map) {
    for (int i = 0; i < map.h; i++) {
        for (int j = 0; j < map.w; j++) {
            if (map.tile[i][j] == WALL) {
                v2 a, b, c, d;
                a = V2(j * TILE_SIZE, i * TILE_SIZE);
                b = V2(j * TILE_SIZE + TILE_SIZE, i * TILE_SIZE);
                c = V2(j * TILE_SIZE + TILE_SIZE, i * TILE_SIZE + TILE_SIZE);
                d = V2(j * TILE_SIZE, i * TILE_SIZE + TILE_SIZE);
                if (rect_intersects_circle(pos, r, a, b, c, d)) {
                    return true;
                }
            }
        }
    }

    return false;
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
                    player->previous_pos = player->pos;
                    velocity = math_normalize(velocity) * player->speed * dt;
                    v2 next_pos_h = V2(velocity.x, 0);
                    v2 next_pos_v = V2(0, velocity.y);
                    if (!collides_with_walls(next_pos_v + player->pos, player->hitbox_r, game_state->map[game_state->current_map_id])) {
                        player->pos += next_pos_v;
                    }
                    if (!collides_with_walls(next_pos_h + player->pos, player->hitbox_r, game_state->map[game_state->current_map_id])) {
                        player->pos += next_pos_h;
                    }
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
            for (int j = 0; j < m.h; j++) {
                for (int i = 0; i < m.w; i++) {
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
                            SDL_RenderCopy(renderer, m.door_sprite, 0, &rect);
                            break;
                        default:
                            assert(false);
                            break;
                    }
                }
            }

            // draw enemies
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

                v2 v = player.pos - player.previous_pos;
                double angle = atan2(v.y, v.x);
                angle *= 180/M_PI;
                SDL_RenderCopyEx(renderer, player.image, 0, &rect, angle, NULL, SDL_FLIP_NONE);
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
