enum entity_type_e
{
    ENTITY_BALL,
    ENTITY_ENEMY,
};

enum rotation_type_e
{
    ROTATE_CW = 0,
    ROTATE_CCW,
};

typedef struct
{
    double red;
    double green;
    double blue;
    double alpha;
} color_t;

// TODO: make sure these attacks can be used more than once (preserve initial state)
//       (not sure if needed)
typedef struct
{
    v2 *spawn_loc;
    double spawn_rate;             // in milliseconds
    double time_since_last_spawn;  // in milliseconds
    int particles_per_spawn;
    double arc_center;
    double arc_size;
    rotation_type_e rotation_type;
    double angle_step;
    double last_angle; // TODO: remember to reset this once the attack is finished
    double particle_speed;
    v2 particle_accel;
    const char *particle_image;
    int particle_width;
    int particle_height;
    v3 particle_color;
    // TODO: maybe add a pointer to function that can be called every in particle's update
} atk_pattern_t;

typedef struct
{
    bool stopped;
    bool forced_movement;
    v2 target_loc;
    v2 path[10];
    int path_size;

    double time_since_fight_started;

    // TODO: change this to a linked list
    // NOTE: attacks do not necessarily happen in order, they can even happen simultaneously
    atk_pattern_t atks[10];
} enemy_only_t;

typedef struct
{
    v2 pos;
    double speed;
    // TODO: consider adding velocity and acceleration here (instead of speed)

    const char *image_path;
    int w;
    int h;

    double health;

    entity_type_e type;
    union
    {
        enemy_only_t enemy_data;
    };
} entity_t;

typedef struct
{
    v2 pos;
    v2 velocity;
    v2 acceleration;

    // TODO: make this be only a path so that we don't need the renderer to update the game
    // TODO: make sure the renderer does not render the same image multiple times (hash_table)
    const char *image_path;
    int w;
    int h;
    v3 color;
} particle_t;

typedef struct
{
    color_t background_color;

    entity_t mouse_ball;
    entity_t keybd_ball;
    entity_t enemy;

    std::list<particle_t> *particles;
} game_state_t;
