void create_wall_lines(map_t *map) {
    map->hitbox = (seg_t *) malloc(5000 * sizeof(seg_t));
    map->hitbox_size = 0;
    bool idle = true;
    bool has_size = false;
    v2 a;
    v2 b;

    // bottom up lines
    for (int i = 1; i < map->h; i++) {
        for (int j = 0; j < map->w; j++) {
            if (map->tile[i-1][j] == WALL || map->tile[i-1][j] == LOCK) {
                if (idle && map->tile[i][j] != WALL && map->tile[i][j] != LOCK) { // set starting a and b
                    idle = false;
                    a = V2(j * TILE_SIZE, i * TILE_SIZE);
                    b = V2(j * TILE_SIZE + TILE_SIZE, i * TILE_SIZE);
                    has_size = true;
                } else { // update b
                    if (map->tile[i][j] != WALL && map->tile[i][j] != LOCK) {
                        b = V2(j * TILE_SIZE + TILE_SIZE, i * TILE_SIZE);
                        has_size = true;
                    } else if (has_size) {
                        idle = true;
                        map->hitbox[map->hitbox_size].a = a;
                        map->hitbox[map->hitbox_size].b = b;
                        map->hitbox_size++;
                        has_size = false;
                    }
                }
                if (j == map->w-1 && has_size) { // this might be a problem
                    idle = true;
                    map->hitbox[map->hitbox_size].a = a;
                    map->hitbox[map->hitbox_size].b = b;
                    map->hitbox_size++;
                    has_size = false;
                }
            } else {
                if(!idle && has_size) {
                    idle = true;
                    map->hitbox[map->hitbox_size].a = a;
                    map->hitbox[map->hitbox_size].b = b;
                    map->hitbox_size++;
                    has_size = false;
                }
            }
        }
    }
    
    // top down lines
    has_size = false;
    idle = true;
    for (int i = map->h-2; i >= 0; i--) {
        for (int j = 0; j < map->w; j++) {
            if (map->tile[i+1][j] == WALL || map->tile[i+1][j] == LOCK) {
                if (idle && map->tile[i][j] != WALL && map->tile[i][j] != LOCK) { // set starting a and b
                    idle = false;
                    a = V2(j * TILE_SIZE, i * TILE_SIZE + TILE_SIZE);
                    b = V2(j * TILE_SIZE + TILE_SIZE, i * TILE_SIZE + TILE_SIZE);
                    has_size = true;
                } else { // update b
                    if (map->tile[i][j] != WALL && map->tile[i][j] != LOCK) {
                        b = V2(j * TILE_SIZE + TILE_SIZE, i * TILE_SIZE + TILE_SIZE);
                        has_size = true;
                    } else if (has_size) {
                        idle = true;
                        map->hitbox[map->hitbox_size].a = a;
                        map->hitbox[map->hitbox_size].b = b;
                        map->hitbox_size++;
                        has_size = false;
                    }
                }
                if (j == map->w-1 && has_size) {
                    idle = true;
                    map->hitbox[map->hitbox_size].a = a;
                    map->hitbox[map->hitbox_size].b = b;
                    map->hitbox_size++;
                    has_size = false;
                }
            } else {
                if(!idle && has_size) {
                    idle = true;
                    map->hitbox[map->hitbox_size].a = a;
                    map->hitbox[map->hitbox_size].b = b;
                    map->hitbox_size++;
                    has_size = false;
                }
            }
        }
    }

    // right to left lines
    has_size = false;
    idle = true;
    for (int j = 1; j < map->w; j++) {
        for (int i = 0; i < map->h; i++) {
            if (map->tile[i][j-1] == WALL || map->tile[i][j-1] == LOCK) {
                if (idle && map->tile[i][j] != WALL && map->tile[i][j] != LOCK) { // set starting a and b
                    idle = false;
                    a = V2(j * TILE_SIZE, i * TILE_SIZE);
                    b = V2(j * TILE_SIZE, i * TILE_SIZE + TILE_SIZE);
                    has_size = true;
                } else { // update b
                    if (map->tile[i][j] != WALL && map->tile[i][j] != LOCK) {
                        b = V2(j * TILE_SIZE, i * TILE_SIZE + TILE_SIZE);
                        has_size = true;
                    } else if (has_size) {
                        idle = true;
                        map->hitbox[map->hitbox_size].a = a;
                        map->hitbox[map->hitbox_size].b = b;
                        map->hitbox_size++;
                        has_size = false;
                    }
                }
                if(i == map->h-1 && has_size) {
                    idle = true;
                    map->hitbox[map->hitbox_size].a = a;
                    map->hitbox[map->hitbox_size].b = b;
                    map->hitbox_size++;
                    has_size = false;
                }
            } else {
                if(!idle && has_size) {
                    idle = true;
                    map->hitbox[map->hitbox_size].a = a;
                    map->hitbox[map->hitbox_size].b = b;
                    map->hitbox_size++;
                    has_size = false;
                }
            }
        }
    }

    // left to right lines
    has_size = false;
    idle = true;
    for (int j = map->w-2; j >= 0; j--) {
        for (int i = 0; i < map->h; i++) {
            if (map->tile[i][j+1] == WALL || map->tile[i][j+1] == LOCK) {
                if (idle && map->tile[i][j] != WALL && map->tile[i][j] != LOCK) { // set starting a and b
                    idle = false;
                    a = V2(j * TILE_SIZE + TILE_SIZE, i * TILE_SIZE);
                    b = V2(j * TILE_SIZE + TILE_SIZE, i * TILE_SIZE + TILE_SIZE);
                    has_size = true;
                } else { // update b
                    if (map->tile[i][j] != WALL && map->tile[i][j] != LOCK) {
                        b = V2(j * TILE_SIZE + TILE_SIZE, i * TILE_SIZE + TILE_SIZE);
                        has_size = true;
                    } else if (has_size) {
                        idle = true;
                        map->hitbox[map->hitbox_size].a = a;
                        map->hitbox[map->hitbox_size].b = b;
                        map->hitbox_size++;
                        has_size = false;
                    }
                }
                if(i == map->h-1 && has_size) {
                    idle = true;
                    map->hitbox[map->hitbox_size].a = a;
                    map->hitbox[map->hitbox_size].b = b;
                    map->hitbox_size++;
                    has_size = false;
                }
            } else {
                if(!idle && has_size) {
                    idle = true;
                    map->hitbox[map->hitbox_size].a = a;
                    map->hitbox[map->hitbox_size].b = b;
                    map->hitbox_size++;
                    has_size = false;
                }
            }
        }
    }

    /*
    //printf("hitbox size %d\n", map->hitbox_size);
    for (int i = 0; i < map->hitbox_size; i++) {
        //printf("%d) %lf %lf - %lf %lf\n", i, map->hitbox[i].a.x, map->hitbox[i].a.y, map->hitbox[i].b.x, map->hitbox[i].b.y);
    }
    */
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
    for (int i = 0; i < map.hitbox_size; i++) {
        if (seg_intersects_circle(pos, r, map.hitbox[i].a, map.hitbox[i].b)) {
            return true;
        }
    }

    return false;
}

