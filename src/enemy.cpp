void enemy_goto(entity_t *enemy, double dt) {
    v2 dir = enemy->enemy_data.destination - enemy->pos;

    double mag = math_magnitude(dir);
    if (mag == 0) {
        enemy->enemy_data.destination = V2(rand() % 800, rand() % 600);
        return;
    }
    if (mag < 1) {
        enemy->pos = enemy->enemy_data.destination;
        return;
    }

    dir = math_normalize(dir) * (enemy->speed*dt);

    enemy->pos += dir;
}
