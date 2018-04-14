void enemy_goto(entity_t *enemy, v2 dest, double dt) {
    v2 dir = dest - enemy->pos;

    if (math_magnitude(dir) > 0) {
        dir = math_normalize(dir);
    }

    dir *= enemy->speed*dt;

    enemy->pos += dir;
}
