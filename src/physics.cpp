void create_wall_lines(map_t *map) {
    assert(map);

    map->hitbox = (seg_t *) malloc(5000 * sizeof(seg_t));
    map->hitbox_size = 0;
    bool idle = true;
    bool has_size = false;
    v2 a;
    v2 b;

    map->hitbox[map->hitbox_size].a = V2(0, 0);
    map->hitbox[map->hitbox_size].b = V2(0, map->h * TILE_SIZE);
    map->hitbox_size++;
    map->hitbox[map->hitbox_size].a = V2(0, 0);
    map->hitbox[map->hitbox_size].b = V2(map->w * TILE_SIZE, 0);
    map->hitbox_size++;
    map->hitbox[map->hitbox_size].a = V2(map->w * TILE_SIZE, 0);
    map->hitbox[map->hitbox_size].b = V2(map->w * TILE_SIZE, map->h * TILE_SIZE);
    map->hitbox_size++;
    map->hitbox[map->hitbox_size].a = V2(0, map->h * TILE_SIZE);
    map->hitbox[map->hitbox_size].b = V2(map->w * TILE_SIZE, map->h * TILE_SIZE);
    map->hitbox_size++;

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

char get_line_intersection(v2 a, v2 b, v2 c, v2 d, v2 *out) {
    float num = (d.x - c.x) * (a.y - c.y) - (d.y - c.y) * (a.x - c.x);
    if (abs(num) <= 0.00001) {
        return 0;
    }
    float den = (d.y - c.y) * (b.x - a.x) - (d.x - c.x) * (b.y - a.y);
    if (abs(den) <= 0.00001) {
        return 0;
    }
    float num_b = (b.x - a.x) * (a.y - c.y) - (b.y - a.y) * (a.x - c.x);
    if (abs(num_b) <= 0.00001) {
        return 0;
    }
    float u_a = num / den;
    float u_b = num_b / den;
    if (u_a >= 0 && u_a <= 1 && u_b >= 0 && u_b <= 1) {
        if (out) {
            out->x = a.x + u_a * (b.x - a.x);
            out->y = a.y + u_a * (b.y - a.y);
        }
        return 1;
    }
    return 0;
}

// TODO: check if it breaks for two vertical lines and similar cases
// Returns 1 if the lines intersect, otherwise 0. In addition, if the lines
// intersect the intersection point may be stored in the floats i_x and i_y.
char get_line_intersection(float p0_x, float p0_y, float p1_x, float p1_y,
    float p2_x, float p2_y, float p3_x, float p3_y, double *i_x, double *i_y)
{
    float s1_x, s1_y, s2_x, s2_y;
    s1_x = p1_x - p0_x;     s1_y = p1_y - p0_y;
    s2_x = p3_x - p2_x;     s2_y = p3_y - p2_y;

    float s, t;
    s = (-s1_y * (p0_x - p2_x) + s1_x * (p0_y - p2_y)) / (-s2_x * s1_y + s1_x * s2_y);
    t = ( s2_x * (p0_y - p2_y) - s2_y * (p0_x - p2_x)) / (-s2_x * s1_y + s1_x * s2_y);

    if (s >= 0 && s <= 1 && t >= 0 && t <= 1)
    {
        // Collision detected
        if (i_x != NULL)
            *i_x = p0_x + (t * s1_x);
        if (i_y != NULL)
            *i_y = p0_y + (t * s1_y);
        return 1;
    }

    return 0; // No collision
}

bool collides_with_walls(v2 a, v2 b, map_t map, v2 *min_p) {
    bool collides = false;
    double min_dist = -1;
    for (int i = 0; i < map.hitbox_size; i++) {
        v2 ip;
        /*char intersects = get_line_intersection(a.x, a.y, b.x, b.y,
                                                map.hitbox[i].a.x, map.hitbox[i].a.y,
                                                map.hitbox[i].b.x, map.hitbox[i].b.y, &ip.x, &ip.y);
                                                */
        char intersects = get_line_intersection(a, b, map.hitbox[i].a, map.hitbox[i].b, &ip);
        if (intersects) {
            double mag = math_magnitude(ip-a);
            if (mag < min_dist || min_dist == -1) {
                min_dist = mag;
                min_p->x = ip.x;
                min_p->y = ip.y;
            }
            collides = true;
        }
    }

    return collides;
}
