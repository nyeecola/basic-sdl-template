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
    double speed;
    SDL_Texture *image;
    int w;
    int h;
} ball_t;

typedef struct
{
    color_t background_color;

    ball_t mouse_ball;   
    ball_t keybd_ball;   
} game_state_t;
