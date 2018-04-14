void enemy_goto(entity_t *enemy, v2 dest, double dt) {
    v2 dir = dest - enemy->pos;

    double mag = math_magnitude(dir);
    if (mag == 0) {
        return;
    }
    if (mag < 1) {
        enemy->pos = dest;
        return;
    }

    dir = math_normalize(dir) * (enemy->speed*dt);

    enemy->pos += dir;
}
