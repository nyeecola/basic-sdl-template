void enemy_set_destination(map_t map, entity_t *enemy, v2 dest) {
    int tiles[30][40];

    for (int j = 0; j < 30; j++) {
        for (int i = 0; i < 40; i++) {
            tiles[j][i] = (map.tile[j][i] == WALL || map.tile[j][i] == LOCK) ? -2 : -1;
        }
    }

    dest = V2(((int) dest.x)/TILE_SIZE, ((int) dest.y)/TILE_SIZE);
    //printf("dest %lf %lf\n", dest);
    //dest = dest/TILE_SIZE;

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
    enemy->enemy_data.path[0] = V2(((int) enemy_pos.x)/TILE_SIZE, ((int) enemy_pos.y)/TILE_SIZE);

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

void enemy_move(map_t map, entity_t *enemy, double dt) {
    enemy->previous_pos = enemy->pos;

    v2 dir = (enemy->enemy_data.path[enemy->enemy_data.path_cur]*TILE_SIZE) + V2(TILE_SIZE/2, TILE_SIZE/2) - enemy->pos;
    double mag = math_magnitude(dir);

    if (mag < 4) {
        enemy->pos = (enemy->enemy_data.path[enemy->enemy_data.path_cur]*TILE_SIZE) + V2(TILE_SIZE/2, TILE_SIZE/2);
        if (enemy->enemy_data.path_cur+1 != enemy->enemy_data.path_len) {
            enemy->enemy_data.path_cur++;
        }
        else {
            enemy_set_destination(map, enemy, enemy->enemy_data.possibleDestinations[rand()%enemy->enemy_data.possibleDestinations_len]);
        }
        return;
    }

    v2 normalized_dir = math_normalize(dir);
    dir = normalized_dir * (enemy->speed*dt);

    v2 cur_dir;
    cur_dir.x = cos(enemy->angle/(180.0f/M_PI));
    cur_dir.y = sin(enemy->angle/(180.0f/M_PI));
    float dot = normalized_dir * math_normalize(cur_dir);

#if 0
    if (dot >= 0.9999 && dot <= 1.0001) {
        enemy->pos += dir;
    } else {
        float target_angle = atan2(dir.y, dir.x);
        target_angle *= 180.0f/M_PI;

        float a = target_angle;
        if (a < 0) a += 360;
        float b = enemy->angle;
        if (b < 0) b += 360;
        float c = a - b;
        if (c < 0) c += 360;

        if (c < 180) {
            enemy->angle += enemy->enemy_data.rotation_speed * dt;
        } else {
            enemy->angle -= enemy->enemy_data.rotation_speed * dt;
        }
    }
#else
    if (dot >= 0.88 && dot <= 1.12) {
        enemy->pos += dir;
    }

    if (dot <= 0.999 || dot >= 1.001) {
        float target_angle = atan2(dir.y, dir.x);
        target_angle *= 180.0f/M_PI;

        float a = target_angle;
        if (a < 0) a += 360;
        float b = enemy->angle;
        if (b < 0) b += 360;
        float c = a - b;
        if (c < 0) c += 360;

        if (c < 180) {
            enemy->angle += enemy->enemy_data.rotation_speed * dt;
            if (enemy->angle > 360) enemy->angle -= 360;
            if (enemy->angle < 0) enemy->angle += 360;
        } else {
            enemy->angle -= enemy->enemy_data.rotation_speed * dt;
            if (enemy->angle > 360) enemy->angle -= 360;
            if (enemy->angle < 0) enemy->angle += 360;
        }
    }
#endif
}
