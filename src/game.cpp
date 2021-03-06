void add_enemy_to_map(map_t *map, SDL_Renderer *renderer, v2 *vecs, int n) {
    assert(n >= 2);

    int index = map->enemies_count;
    map->enemies[index].pos = vecs[rand()%n] * TILE_SIZE;
    map->enemies[index].speed = 100;
    map->enemies[index].image = IMG_LoadTexture(renderer, ENEMY_IMG_PATH);
    map->enemies[index].image_w = 18;
    map->enemies[index].image_h = 18;
    map->enemies[index].type = ENEMY;
    for (int i = 0; i < n; i++) {
        map->enemies[index].enemy_data.possibleDestinations[i] = vecs[i] * TILE_SIZE;
    }
    map->enemies[index].enemy_data.possibleDestinations_len = n;
    map->enemies[index].enemy_data.path = NULL;
    map->enemies[index].enemy_data.rotation_speed = 120;
    enemy_set_destination(*map, &map->enemies[index], vecs[rand()%n] * TILE_SIZE);
    map->enemies_count++;
}

void load_maps(game_state_t *game_state, map_t *map, SDL_Renderer *renderer) {
    FILE *arq = fopen(MAPS_SOURCE_PATH, "r");

    int trash;
    char current;

    trash = fscanf(arq, "%d", &game_state->maps);
    assert(game_state->maps <= MAX_MAPS_PER_RUN);

    for(int m=0 ; m < game_state->maps ; m++) {
        v2 vecs[30][30];
        int ns[30] = {0};

        map[m].enemies_count = 0;
        trash = fscanf(arq, " %d %d %d", &map[m].w, &map[m].h, &map[m].doors);
        do {
            //trash = fscanf(arq, "%c", &current);
            current = fgetc(arq);
        } while ( current != '\n');
        assert(map[m].w <= 40);
        assert(map[m].h <= 30);
        assert(map[m].doors <= MAX_DOOR_PER_ROOM);
        for(int y=0 ; y < map[m].h ; y++) {
            for(int x=0 ; x < map[m].w ; x++) {
                //trash = fscanf(arq, " %c", &current);
                current = fgetc(arq);
                switch(current) {
                    case ' ': {
                        map[m].tile[y][x] = EMPTY;
                        break;
                    }
                    case 'O': {
                        map[m].tile[y][x] = WALL;
                        break;
                    }
                    case 'L': {
                        map[m].tile[y][x] = LOCK;
                        break;
                    }
                    case 'P': {
                        map[m].tile[y][x] = PASSWORD;
                        break;
                    }
                    case 'S': {
                        map[m].tile[y][x] = LASER_SOURCE;
                        break;
                    }
                    default: {
                        if (current >= 'a' && current <= 'z') {
                            map[m].tile[y][x] = EMPTY;
                            int index = current - 'a';
                            vecs[index][ns[index]] = V2(x, y);
                            ns[index]++;
                        } else {
                            printf("%d %d\n", x, y);
                            printf("%c\n", current);
                            assert(false);
                        }
                        break;
                    }
                }
            }
            do {
                //trash = fscanf(arq, "%c", &current);
                current = fgetc(arq);
            } while ( current != '\n');
        }

        for (int i = 0; i < 30; i++) {
            if (ns[i] == 0) continue;
            add_enemy_to_map(&map[m], renderer, vecs[i], ns[i]);
        }

        for(int i=0 ; i < map[m].doors ; i++) {
            trash = fscanf(arq, "%d %d", &map[m].door[i].x, &map[m].door[i].y);
            trash = fscanf(arq, "%d %d", &map[m].door[i].exit_x, &map[m].door[i].exit_y);
            trash = fscanf(arq, "%d %d", &map[m].door[i].target_map, &map[m].door[i].target_door);
            map[m].tile[map[m].door[i].y][map[m].door[i].x] = DOOR;
        }
    }
    trash++;

    fclose(arq);

    for(int a=0 ; a < game_state->maps ; a++) {
        map[a].wall_sprite = IMG_LoadTexture(renderer, WALL_IMG_PATH);
        assert(map[a].wall_sprite);
        map[a].floor_sprite = IMG_LoadTexture(renderer, FLOOR_IMG_PATH);
        assert(map[a].floor_sprite);
        map[a].password_sprite = IMG_LoadTexture(renderer, PASSWORD_IMG_PATH);
        assert(map[a].floor_sprite);
        map[a].doorh_sprite = IMG_LoadTexture(renderer, DOORH_IMG_PATH);
        assert(map[a].doorh_sprite);
        map[a].lockh_sprite = IMG_LoadTexture(renderer, LOCKH_IMG_PATH);
        assert(map[a].lockh_sprite);
        map[a].doorw_sprite = IMG_LoadTexture(renderer, DOORW_IMG_PATH);
        assert(map[a].doorw_sprite);
        map[a].lockw_sprite = IMG_LoadTexture(renderer, LOCKW_IMG_PATH);
        assert(map[a].lockw_sprite);
        map[a].laser_source_sprite = IMG_LoadTexture(renderer, LASER_SOURCE_IMG_PATH);
        assert(map[a].laser_source_sprite);

        create_wall_lines(&game_state->map[a]);
    }
}

void game_state_initialize(game_state_t *game_state, SDL_Renderer *renderer) {
    assert(renderer);

    // seed
    srand(123);

    // game mode
    game_state->game_mode = PLAYING;

    // finale
    game_state->finale = false;

    // background_color
    game_state->background_color.r = 0;
    game_state->background_color.g = 0;
    game_state->background_color.b = 127;
    game_state->background_color.a = 255;

    // initialize maps
    game_state->map = (map_t*) malloc(sizeof(*(game_state->map)) * MAX_DOOR_PER_ROOM);
    //game_state->current_map_id = 2;
    game_state->current_map_id = game_state->spawn_map_id;
    load_maps(game_state, game_state->map, renderer);

    // initialize player data
    //game_state->player.pos = V2(750, 550);
    //game_state->player.pos = V2(50, 50);
    game_state->player.pos = game_state->spawn_loc;
    game_state->player.speed = 150;
    game_state->player.image = IMG_LoadTexture(renderer, CAT_IMG_PATH);
    game_state->player.image2 = IMG_LoadTexture(renderer, CAT_PASSWORD_IMG_PATH);
    //SDL_QueryTexture(game_state->player.image, 0, 0,
                     //&game_state->player.image_w, &game_state->player.image_h);
    game_state->player.image_w = 18;
    game_state->player.image_h = 18;
    game_state->player.type = PLAYER;
    game_state->player.hitbox_r = game_state->player.image_w/2;
    game_state->player.player_data.has_password = false;
}

void game_state_restart(game_state_t *game_state, SDL_Renderer *renderer) {
    for (int k = 0; k < game_state->maps; k++) {
        free(game_state->map[k].hitbox);
        SDL_DestroyTexture(game_state->map[k].wall_sprite);
        SDL_DestroyTexture(game_state->map[k].floor_sprite);
        SDL_DestroyTexture(game_state->map[k].doorw_sprite);
        SDL_DestroyTexture(game_state->map[k].doorh_sprite);
        SDL_DestroyTexture(game_state->map[k].lockw_sprite);
        SDL_DestroyTexture(game_state->map[k].lockh_sprite);
        SDL_DestroyTexture(game_state->map[k].password_sprite);
        SDL_DestroyTexture(game_state->map[k].laser_source_sprite);
        for (int q = 0; q < game_state->map[k].enemies_count; q++) {
            if (game_state->map[k].enemies[q].enemy_data.path) {
                free(game_state->map[k].enemies[q].enemy_data.path);
            }
            if (game_state->map[k].enemies[q].image) {
                SDL_DestroyTexture(game_state->map[k].enemies[q].image);
            }
            if (game_state->map[k].enemies[q].image2) {
                SDL_DestroyTexture(game_state->map[k].enemies[q].image2);
            }
        }
    }
    free(game_state->map);
    if (game_state->player.image) SDL_DestroyTexture(game_state->player.image);
    if (game_state->player.image2) SDL_DestroyTexture(game_state->player.image2);
    game_state_initialize(game_state, renderer);
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
                //create_wall_lines(&game_state->map[game_state->current_map_id]);
                door_t door = map.door[d];

                game_state->player.pos.x = door.exit_x * TILE_SIZE + TILE_SIZE/2;
                game_state->player.pos.y = door.exit_y * TILE_SIZE + TILE_SIZE/2;

                if (game_state->current_map_id > game_state->spawn_map_id) {
                    game_state->spawn_map_id = game_state->current_map_id;
                }
                break;
            }
        }
    }
}

void faced_tile(game_state_t *gamestate, int *x, int *y) {
    int ang = (int) gamestate->player.angle;
    v2 player_tile = entity_tile_pos(gamestate->player);

    if ( ang == 0 ) {
        *x = (int)player_tile.x + 1;
        *y = (int)player_tile.y;
    } else if ( ang == 180 ) {
        *x = (int)player_tile.x - 1;
        *y = (int)player_tile.y;
    } else if ( ang == -90 ) {
        *x = (int)player_tile.x;
        *y = (int)player_tile.y - 1;
    } else {
        *x = (int)player_tile.x;
        *y = (int)player_tile.y + 1;
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
                // finale
                if (!game_state->finale &&
                        game_state->current_map_id == 2 &&
                        player->pos.y >= 11 * TILE_SIZE &&
                        player->pos.x >= 37 * TILE_SIZE &&
                        player->pos.y <= 13 * TILE_SIZE &&
                        player->pos.x <= 39 * TILE_SIZE) {
                    game_state->finale = true;
                    game_state->finale_timer = 5;
                }
                if (game_state->finale_timer >= 0) game_state->finale_timer -= dt;

                // movement
                v2 velocity = V2(0,0);
                if (input->keys_pressed[SDL_SCANCODE_W]) {
                    velocity += V2(0,-1);
                }
                if (input->keys_pressed[SDL_SCANCODE_S]) {
                    velocity += V2(0,1);
                }
                if (input->keys_pressed[SDL_SCANCODE_A]) {
                    velocity += V2(-1,0);
                }
                if (input->keys_pressed[SDL_SCANCODE_D]) {
                    velocity += V2(1,0);
                }
                if (input->keys_pressed[SDL_SCANCODE_E]) {
                    if ( game_state->player.player_data.has_password ) {
                        int x,y;
                        //faced_tile(game_state, &x, &y);
                        v2 player_tile = entity_tile_pos(game_state->player);
                        x = player_tile.x;
                        y = player_tile.y;
                        map_t *map = &(game_state->map[game_state->current_map_id]);
                        if ( map->tile[y+1][x] == LOCK ) {
                            map->tile[y+1][x] = EMPTY;
                            create_wall_lines(map);
                            game_state->player.player_data.has_password = false;
                            SDL_Texture *aux = game_state->player.image;        
                            game_state->player.image = game_state->player.image2;       
                            game_state->player.image2 = aux;
                        } else if ( map->tile[y-1][x] == LOCK ) {
                            map->tile[y-1][x] = EMPTY;
                            create_wall_lines(map);
                            game_state->player.player_data.has_password = false;
                            SDL_Texture *aux = game_state->player.image;        
                            game_state->player.image = game_state->player.image2;       
                            game_state->player.image2 = aux;
                        } else if ( map->tile[y][x+1] == LOCK ) {
                            map->tile[y][x+1] = EMPTY;
                            create_wall_lines(map);
                            game_state->player.player_data.has_password = false;
                            SDL_Texture *aux = game_state->player.image;        
                            game_state->player.image = game_state->player.image2;       
                            game_state->player.image2 = aux;
                        } else if ( map->tile[y][x-1] == LOCK ) {
                            map->tile[y][x-1] = EMPTY;
                            create_wall_lines(map);
                            game_state->player.player_data.has_password = false;
                            SDL_Texture *aux = game_state->player.image;        
                            game_state->player.image = game_state->player.image2;       
                            game_state->player.image2 = aux;
                        }
                    }
                }
                if (math_magnitude(velocity)) {
                    player->previous_pos = player->pos;
                    velocity = math_normalize(velocity) * player->speed * dt;
                    v2 next_pos_h = V2(velocity.x, 0);
                    v2 next_pos_v = V2(0, velocity.y);
                    if (!collides_with_walls(next_pos_v + player->pos, player->hitbox_r,
                                             game_state->map[game_state->current_map_id])) {
                        player->pos += next_pos_v;
                    }
                    if (!collides_with_walls(next_pos_h + player->pos, player->hitbox_r,
                                             game_state->map[game_state->current_map_id])) {
                        player->pos += next_pos_h;
                    }

                    v2 v = player->pos - player->previous_pos;
                    if (math_magnitude(v)) {
                        player->angle = atan2(v.y, v.x);
                        player->angle *= 180/M_PI;
                    }
                }

                // update enemies
#if 0
                int e_c = game_state->enemies_count;
                for (int i = 0; i < e_c; i++) {
#if 0
                    v2 v = game_state->enemies[i].pos - game_state->enemies[i].previous_pos;
                    if (math_magnitude(v)) {
                        game_state->enemies[i].angle = atan2(v.y, v.x);
                        game_state->enemies[i].angle *= 180/M_PI;
                    }
#endif

                    enemy_move(game_state->map[game_state->current_map_id], &game_state->enemies[i], dt);
                }
#endif
                map_t *map = &(game_state->map[game_state->current_map_id]);
                int e_c = map->enemies_count;
                for (int i = 0; i < e_c; i++) {
                    enemy_move(*map, &map->enemies[i], dt);
                }

                v2 tile_pos = entity_tile_pos(game_state->player);
                if ( map->tile[(int)tile_pos.y][(int)tile_pos.x] == PASSWORD && !game_state->player.player_data.has_password ) {
                    map->tile[(int)tile_pos.y][(int)tile_pos.x] = EMPTY;
                    game_state->player.player_data.has_password = true;
                    SDL_Texture *aux = game_state->player.image;      
                    game_state->player.image = game_state->player.image2;       
                    game_state->player.image2 = aux;

                }
                handle_doors(game_state);
            }
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
            {
                v2 laser_source_pos;
                bool has_laser_source = false;
                for (int j = 0; j < m.h; j++) {
                    for (int i = 0; i < m.w; i++) {
                        SDL_Rect rect;
                        rect.x = TILE_SIZE * i;
                        rect.y = TILE_SIZE * j;
                        rect.w = TILE_SIZE;
                        rect.h = TILE_SIZE;
                        switch(m.tile[j][i]) {
                            case EMPTY:
                                SDL_RenderCopy(renderer, m.floor_sprite, 0, &rect);
                                break;
                            case WALL:
                                SDL_RenderCopy(renderer, m.wall_sprite, 0, &rect);
                                break;
                            case DOOR:
                                if ( i == 0 || i == 39 ) {
                                    SDL_RenderCopy(renderer, m.doorh_sprite, 0, &rect);
                                } else {
                                    SDL_RenderCopy(renderer, m.doorw_sprite, 0, &rect); 
                                }
                                break;
                            case PASSWORD:
                                SDL_RenderCopy(renderer, m.password_sprite, 0, &rect);
                                break;
                            case LASER_SOURCE:
                                laser_source_pos = V2(i, j);
                                has_laser_source = true;
                                break;
                            case LOCK:
                                break;
                            default:
                                assert(false);
                                break;
                        }
                    }
                }
                for (int j = 0; j < m.h; j++) {
                    for (int i = 0; i < m.w; i++) {
                        SDL_Rect rect;
                        rect.x = TILE_SIZE * i;
                        rect.y = TILE_SIZE * j;
                        rect.w = TILE_SIZE;
                        rect.h = TILE_SIZE;
                        switch(m.tile[j][i]) {
                            case LOCK:
                                {
                                    if ( m.tile[j-1][i] == WALL || m.tile[j+1][i]) {
                                        SDL_RenderCopy(renderer, m.lockh_sprite, 0, &rect);

                                        if (!game_state->finale ||
                                                (game_state->finale_timer >= 4.5 &&
                                                 game_state->finale_timer <= 4.7) ||
                                                (game_state->finale_timer >= 4.1 &&
                                                 game_state->finale_timer <= 4.3) ||
                                                (game_state->finale_timer >= 3.7 &&
                                                 game_state->finale_timer <= 3.9) ||
                                                (game_state->finale_timer >= 3.3 &&
                                                 game_state->finale_timer <= 3.5) ||
                                                (game_state->finale_timer >= 2.9 &&
                                                 game_state->finale_timer <= 3.1))
                                        {
                                            v2 v;
                                            // TODO: maybe optimize this
                                            v2 t;

                                            if (m.tile[j-1][i] == EMPTY) {
                                                v = V2(0, -1);
                                                v2 pos = V2(rect.x + TILE_SIZE/2, rect.y - 1);
                                                collides_with_walls(pos, pos + v*800, m, &t);
                                                thickLineRGBA(renderer, pos.x, pos.y, t.x, t.y, 4, 255, 0, 120, 255);

                                                // NOTE: quick shitty implementation, should actually be in game_state_update
                                                if (seg_intersects_circle(game_state->player.pos, game_state->player.hitbox_r, pos, t)) {
                                                    game_state_restart(game_state, renderer);
                                                }
                                            }

                                            if (m.tile[j+1][i] == EMPTY) {
                                                v = V2(0, 1);
                                                v2 pos = V2(rect.x + TILE_SIZE/2, rect.y + TILE_SIZE + 1);
                                                collides_with_walls(pos, pos + v*800, m, &t);
                                                thickLineRGBA(renderer, pos.x, pos.y, t.x, t.y, 4, 255, 0, 120, 255);

                                                // NOTE: quick shitty implementation, should actually be in game_state_update
                                                if (seg_intersects_circle(game_state->player.pos, game_state->player.hitbox_r, pos, t)) {
                                                    game_state_restart(game_state, renderer);
                                                }
                                            }
                                        }
                                    } else {
                                        SDL_RenderCopy(renderer, m.lockw_sprite, 0, &rect); 

                                        if (!game_state->finale ||
                                                (game_state->finale_timer >= 4.5 &&
                                                 game_state->finale_timer <= 4.7) ||
                                                (game_state->finale_timer >= 4.1 &&
                                                 game_state->finale_timer <= 4.3) ||
                                                (game_state->finale_timer >= 3.7 &&
                                                 game_state->finale_timer <= 3.9) ||
                                                (game_state->finale_timer >= 3.3 &&
                                                 game_state->finale_timer <= 3.5) ||
                                                (game_state->finale_timer >= 2.9 &&
                                                 game_state->finale_timer <= 3.1))
                                        {
                                            v2 v;
                                            // TODO: maybe optimize this
                                            v2 t;

                                            if (m.tile[j][i+1] == EMPTY) {
                                                v = V2(1, 0);
                                                v2 pos = V2(rect.x + TILE_SIZE + 1, rect.y + TILE_SIZE/2);
                                                collides_with_walls(pos, pos + v*800, m, &t);
                                                thickLineRGBA(renderer, pos.x, pos.y, t.x, t.y, 4, 255, 0, 120, 255);

                                                // NOTE: quick shitty implementation, should actually be in game_state_update
                                                if (seg_intersects_circle(game_state->player.pos, game_state->player.hitbox_r, pos, t)) {
                                                    game_state_restart(game_state, renderer);
                                                }
                                            }

                                            if (m.tile[j][i-1] == EMPTY) {
                                                v = V2(-1, 0);
                                                v2 pos = V2(rect.x - 1, rect.y + TILE_SIZE/2);
                                                collides_with_walls(pos, pos + v*800, m, &t);
                                                thickLineRGBA(renderer, pos.x, pos.y, t.x, t.y, 4, 255, 0, 120, 255);

                                                // NOTE: quick shitty implementation, should actually be in game_state_update
                                                if (seg_intersects_circle(game_state->player.pos, game_state->player.hitbox_r, pos, t)) {
                                                    game_state_restart(game_state, renderer);
                                                }
                                            }
                                        }
                                    }
                                }
                                break;
                            default:
                                break;
                        }
                    }
                }
                if (has_laser_source) {
                    SDL_Rect rect;
                    rect.x = TILE_SIZE * laser_source_pos.x;
                    rect.y = TILE_SIZE * laser_source_pos.y;
                    rect.w = TILE_SIZE * 2;
                    rect.h = TILE_SIZE * 2;
                    SDL_RenderCopy(renderer, m.laser_source_sprite, 0, &rect);
                }
            }

            // draw enemies
            {
                map_t map = game_state->map[game_state->current_map_id];
                int e_c = map.enemies_count;
                entity_t e[128];
                memcpy(&e, map.enemies, sizeof(e));
                for (int i = 0; i < e_c; i++) {
                    // draw laser
                    if (!game_state->finale ||
                            (game_state->finale_timer >= 4.5 && game_state->finale_timer <= 4.7) ||
                            (game_state->finale_timer >= 4.1 && game_state->finale_timer <= 4.3) ||
                            (game_state->finale_timer >= 3.7 && game_state->finale_timer <= 3.9) ||
                            (game_state->finale_timer >= 3.3 && game_state->finale_timer <= 3.5) ||
                            (game_state->finale_timer >= 2.9 && game_state->finale_timer <= 3.1)
                            /*(game_state->finale_timer >= 2.5 && game_state->finale_timer <= 2.7) ||
                            (game_state->finale_timer >= 0.8 && game_state->finale_timer <= 1.8)*/)
                    {
                        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 1);
                        v2 v;
                        /*
                           float m = math_magnitude(e[i].pos - e[i].previous_pos);
                           if (m <= 0.001) {
                           v = V2(1, 0);
                           } else {
                           assert(m > 0);
                           v = math_normalize(e[i].pos - e[i].previous_pos);
                           }
                           */
                        // TODO: maybe optimize this
                        v.x = cos(e[i].angle/(180.0f/M_PI));
                        v.y = sin(e[i].angle/(180.0f/M_PI));
                        v2 t;
                        collides_with_walls(e[i].pos, e[i].pos + v*800, map, &t);

                        // NOTE: quick shitty implementation, should actually be in game_state_update
                        if (!game_state->finale &&
                            seg_intersects_circle(game_state->player.pos,
                                    game_state->player.hitbox_r,
                                    e[i].pos,
                                    t)) {
                            game_state_restart(game_state, renderer);
                        }

                        //SDL_RenderDrawLine(renderer, e[i].pos.x, e[i].pos.y, t.x, t.y);
                        thickLineRGBA(renderer, e[i].pos.x, e[i].pos.y, t.x, t.y, 4, 255, 0, 0, 255);
                    }

                    SDL_Rect rect;
                    rect.x = e[i].pos.x - e[i].image_w/2;
                    rect.y = e[i].pos.y - e[i].image_h/2;
                    rect.w = e[i].image_w;
                    rect.h = e[i].image_h;
                    SDL_RenderCopyEx(renderer, e[i].image, 0, &rect, e[i].angle, NULL, SDL_FLIP_NONE);
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

                SDL_RenderCopyEx(renderer, player.image, 0, &rect, player.angle, NULL, SDL_FLIP_NONE);
            }

#if 0
            // debug draw lines
            {
                SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
                for (int i = 0; i < game_state->map[game_state->current_map_id].hitbox_size; i++) {
                    seg_t line = game_state->map[game_state->current_map_id].hitbox[i];
                    SDL_RenderDrawLine(renderer, line.a.x, line.a.y, line.b.x, line.b.y);
                }
            }
#endif

            if (game_state->finale && game_state->finale_timer <= 1) {
                SDL_Rect rect;
                rect.x = 0;
                rect.y = 0;
                rect.w = DEFAULT_SCREEN_WIDTH;
                rect.h = DEFAULT_SCREEN_HEIGHT;
                the_end_image = IMG_LoadTexture(renderer, THE_END_IMG_PATH);
                SDL_RenderCopy(renderer, the_end_image, 0, &rect);
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
