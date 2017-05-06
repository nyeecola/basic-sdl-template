enum entity_type_t
{
    ENTITY_BALL,
    ENTITY_ENEMY,
};

typedef struct
{
    double red;
    double green;
    double blue;
    double alpha;
} color_t;

typedef struct
{
    bool stopped;
    v2 target_loc;
    v2 path[10];
    int path_size;

    double cooldown;

    // TODO: separe this into an atk_pattern_t struct
    double spawn_rate;             // in milliseconds
    double time_since_last_action; // in milliseconds
    double angle_step;
    // TODO: remember to reset this once the attack is finished
    double last_angle;
    double particle_speed;
} enemy_only_t;

typedef struct
{
    v2 pos;
    double speed;
    // TODO: consider adding velocity and acceleration here

    // TODO: make this be only a path so that we don't need the renderer to update the game
    // TODO: make sure the renderer does not render the same image multiple times (hash_table)
    const char *image_path;
    int w;
    int h;

    entity_type_t type;
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
} particle_t;

typedef struct
{
    color_t background_color;

    entity_t mouse_ball;   
    entity_t keybd_ball;   
    entity_t enemy;

    //particle_t particles[3000]; // TODO: create a linked list for this
    //int num_particles;
    std::list<particle_t> *particles;
} game_state_t;
