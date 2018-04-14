void enemy_set_destination(map_t *map, entity_t *enemy, v2 dest) {
    int tiles[30][40];

    for (int j = 0; j < 30; j++) {
        for (int i = 0; i < 40; i++) {
            tiles[j][i] = map->tile[j][i] == WALL ? -2 : -1;
        }
    }

    dest = dest/TILE_SIZE;

    tiles[(int) dest.y][(int) dest.x] = 0;
    

    v2 queue[30*40];
    int start = 0, end = 0;
    queue[end++] = dest;

    v2 enemy_pos = enemy->pos;
    while (end != start) {
        v2 cur = queue[start++];

        if (cur == enemy_pos) break;

        if (tiles[int(cur.y -1)][int(cur.x -1)] == -1) {
            tiles[int(cur.y -1)][int(cur.x -1)] = tiles[int(cur.y)][int(cur.x)] +1;
            queue[end++] = V2(cur.x -1, cur.y -1);
        }
        if (tiles[int(cur.y -1)][int(cur.x)] == -1) {
            tiles[int(cur.y -1)][int(cur.x)] = tiles[int(cur.y)][int(cur.x)] +1;
            queue[end++] = V2(cur.x, cur.y -1);
        }
        if (tiles[int(cur.y -1)][int(cur.x +1)] == -1) {
            tiles[int(cur.y -1)][int(cur.x +1)] = tiles[int(cur.y)][int(cur.x)] +1;
            queue[end++] = V2(cur.x +1, cur.y -1);
        }
        if (tiles[int(cur.y)][int(cur.x -1)] == -1) {
            tiles[int(cur.y)][int(cur.x -1)] = tiles[int(cur.y)][int(cur.x)] +1;
            queue[end++] = V2(cur.x -1, cur.y);
        }
        if (tiles[int(cur.y)][int(cur.x +1)] == -1) {
            tiles[int(cur.y)][int(cur.x +1)] = tiles[int(cur.y)][int(cur.x)] +1;
            queue[end++] = V2(cur.x +1, cur.y);
        }
        if (tiles[int(cur.y +1)][int(cur.x -1)] == -1) {
            tiles[int(cur.y +1)][int(cur.x -1)] = tiles[int(cur.y)][int(cur.x)] +1;
            queue[end++] = V2(cur.x -1, cur.y +1);
        }
        if (tiles[int(cur.y +1)][int(cur.x)] == -1) {
            tiles[int(cur.y +1)][int(cur.x)] = tiles[int(cur.y)][int(cur.x)] +1;
            queue[end++] = V2(cur.x, cur.y +1);
        }
        if (tiles[int(cur.y +1)][int(cur.x +1)] == -1) {
            tiles[int(cur.y +1)][int(cur.x +1)] = tiles[int(cur.y)][int(cur.x)] +1;
            queue[end++] = V2(cur.x +1, cur.y +1);
        }
    }

    if (enemy->enemy_data.path) {
        free(enemy->enemy_data.path);
    }
    enemy->enemy_data.path = (v2*) malloc(end*sizeof(v2));
    enemy->enemy_data.path[0] = enemy_pos/TILE_SIZE;

    int i = 0;
    int k = 0;
    while (true) {
        v2 currentPoint = enemy->enemy_data.path[i];
        if (tiles[(int)currentPoint.y][(int) currentPoint.x] == 0) break;
        v2 min = currentPoint;

        if (tiles[int(min.y)][int(min.x)] > tiles[int(currentPoint.y+1)][int(currentPoint.x)]) {
            if (tiles[int(currentPoint.y+1)][int(currentPoint.x)] != -2) {
                min = V2(currentPoint.x, currentPoint.y+1);
            }
        }
        if (tiles[int(min.y)][int(min.x)] > tiles[int(currentPoint.y)][int(currentPoint.x-1)]) {
            if (tiles[int(currentPoint.y)][int(currentPoint.x-1)] != -2) {
                min = V2(currentPoint.x-1, currentPoint.y);
            }
        }
        if (tiles[int(min.y)][int(min.x)] > tiles[int(currentPoint.y)][int(currentPoint.x+1)]) {
            if (tiles[int(currentPoint.y)][int(currentPoint.x+1)] != -2) {
                min = V2(currentPoint.x+1, currentPoint.y);
            }
        }
        if (tiles[int(min.y)][int(min.x)] > tiles[int(currentPoint.y-1)][int(currentPoint.x)]) {
            if (tiles[int(currentPoint.y-1)][int(currentPoint.x)] != -2) {
                min = V2(currentPoint.x, currentPoint.y-1);
            }
        }
        if (tiles[int(min.y)][int(min.x)] > tiles[int(currentPoint.y-1)][int(currentPoint.x-1)]) {
            if (tiles[int(currentPoint.y-1)][int(currentPoint.x-1)] != -2) {
                min = V2(currentPoint.x-1, currentPoint.y-1);
            }
        }
        if (tiles[int(min.y)][int(min.x)] > tiles[int(currentPoint.y-1)][int(currentPoint.x+1)]) {
            if (tiles[int(currentPoint.y-1)][int(currentPoint.x+1)] != -2) {
                min = V2(currentPoint.x+1, currentPoint.y-1);
            }
        }
        if (tiles[int(min.y)][int(min.x)] > tiles[int(currentPoint.y+1)][int(currentPoint.x-1)]) {
            if (tiles[int(currentPoint.y+1)][int(currentPoint.x-1)] != -2) {
                min = V2(currentPoint.x-1, currentPoint.y+1);
            }
        }
        if (tiles[int(min.y)][int(min.x)] > tiles[int(currentPoint.y+1)][int(currentPoint.x+1)]) {
            if (tiles[int(currentPoint.y+1)][int(currentPoint.x+1)] != -2) {
                min = V2(currentPoint.x+1, currentPoint.y+1);
            }
        }

        enemy->enemy_data.path[++i] = min;
    }

    enemy->enemy_data.path_len = i+1;
    enemy->enemy_data.path_cur = 0;
}

void enemy_move(map_t *map, entity_t *enemy, double dt) {
    enemy->previous_pos = enemy->pos;

    v2 dir = (enemy->enemy_data.path[enemy->enemy_data.path_cur]*TILE_SIZE) - enemy->pos;
    double mag = math_magnitude(dir);

    if (mag < 4) {
        enemy->pos = (enemy->enemy_data.path[enemy->enemy_data.path_cur]*TILE_SIZE);
        if (enemy->enemy_data.path_cur+1 != enemy->enemy_data.path_len) {
            enemy->enemy_data.path_cur++;
        }
        else {
            enemy_set_destination(map, enemy, enemy->enemy_data.possibleDestinations[rand()%enemy->enemy_data.possibleDestinations_len]);
        }
        return;
    }

    dir = math_normalize(dir) * (enemy->speed*dt);

    enemy->pos += dir;
}