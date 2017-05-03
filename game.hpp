enum entity_type_t
{
    ENTITY_BALL,
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
    double x;
    double y;
    double speed; // TODO: change for velocity

    SDL_Texture *image;
    int w;
    int h;

    entity_type_t type;
} entity_t;

typedef struct
{
    color_t background_color;

    entity_t mouse_ball;   
    entity_t keybd_ball;   
    entity_t enemy_ball;
} game_state_t;
