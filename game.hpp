enum entity_type_t
{
    ENTITY_BALL,
    ENTITY_ENEMY,
};

typedef struct
{
    float red;
    float green;
    float blue;
    float alpha;
} color_t;

typedef struct
{
    bool stopped;
    v2 target_loc;
    v2 path[10];
    int path_size;
} enemy_only_t;

typedef struct
{
    v2 pos;
    double speed;

    SDL_Texture *image;
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
    color_t background_color;

    entity_t mouse_ball;   
    entity_t keybd_ball;   
    entity_t enemy;
} game_state_t;
