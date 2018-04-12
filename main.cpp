#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <cassert>

#include "math.cpp"
#include "debug.cpp"
#include "game.cpp"

// -------------------
// Constants
// -------------------

// screen
const int DEFAULT_SCREEN_WIDTH = 800;
const int DEFAULT_SCREEN_HEIGHT = 600;

// debug text
const char * const FONT_PATH = "assets/Inconsolata-Bold.ttf";
const int FONT_SIZE = 20;

// test assets (ball)
const char * const BALL_IMG_PATH = "assets/ball.png";
const float BALL_SPEED = 0.8;
const float BALL_SCALE = 0.1;

// ---------------------
// Input data
// ---------------------

typedef struct {
    int x;
    int y;
} mouse_t;

typedef struct {
    // keyboard
    const uint8_t *keys_pressed;

    // mouse
    mouse_t mouse;
} input_t;

// --------------------
// Basic data
// --------------------

typedef struct {
    float r;
    float g;
    float b;
    float a;
} color_t;

// ---------------------
// Game data
// ---------------------

enum entity_type_t {
    ENTITY_BALL,
    ENTITY_ENEMY,
};

typedef struct {
    bool stopped;
    v2 target_loc;
    v2 path[10];
    int path_size;
} enemy_only_t;

typedef struct {
    v2 pos;
    double speed;

    SDL_Texture *image;
    int w;
    int h;

    entity_type_t type;
    union {
        enemy_only_t enemy_data;
        // other entity data
    };
} entity_t;

typedef struct {
    // scene data
    color_t background_color;

    // player data
    entity_t mouse_ball;   
    entity_t keybd_ball;   

    // enemies data
    entity_t enemy;
} game_state_t;

typedef struct {

} game_mode_t;

// ---------------------
// Misc functions
// ---------------------

void force_quit(const char *str) {
    puts(str);
    printf("Error: %s\n", SDL_GetError());
    SDL_Quit();
    exit(1);
}

// ---------------------
// Game code
// ---------------------

input_t *initialize_input() {
    input_t *input = (input_t *) malloc(sizeof(*input));

    input->keys_pressed = SDL_GetKeyboardState(0);
    input->mouse.x = 0;
    input->mouse.y = 0;

    return input;
}

int main(int, char *[]) {
    // initialize video
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        force_quit("Failed to initialize SDL.\n");
    }

    // create window
    SDL_Window* window = SDL_CreateWindow("SDL Template",
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          DEFAULT_SCREEN_WIDTH,
                                          DEFAULT_SCREEN_HEIGHT,
                                          SDL_WINDOW_SHOWN);
    if (!window) {
        force_quit("Failed to create window.\n");
    }

    // create renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        SDL_DestroyWindow(window);
        force_quit("Failed to create renderer.\n");
    }

    // initialization
    // TODO: set seed for RNG
    input_t *input = initialize_input();
    game_state_t *game_state = game_state_initialize(renderer);
    debug_initialize_text(renderer);

    // main loop
    bool running = true;
    u64 current_counter = SDL_GetPerformanceCounter();
    u64 last_counter = 0;
    double delta_time = 0;
    while (running) {
        // handle I/O events
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = false;
                    break;
                case SDL_MOUSEMOTION:
                    input->mouse.x = event.motion.x;
                    input->mouse.y = event.motion.y;
                    break;
            }
        }

        // calculate time elapsed since last frame
        last_counter = current_counter;
        current_counter = SDL_GetPerformanceCounter();
        delta_time = (double) ((abs(current_counter - last_counter) * 1000) / SDL_GetPerformanceFrequency());

        // update world for this frame
        game_state_update(game_state, input, delta_time);

        // render frame
        game_state_render(game_state, renderer, delta_time);
        debug_draw_fps(delta_time);
        SDL_RenderPresent(renderer);
    }

    // clean up (needed mostly if the game is fullscreen, since screen resolution goes crazy)
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
