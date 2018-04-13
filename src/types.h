// --------------------
// Math data
// --------------------

typedef struct {
    double x;
    double y;
} v2;

typedef struct {
    double x;
    double y;
    double z;
} v3;

// ---------------------
// Input data
// ---------------------

struct mouse_t {
    int x;
    int y;
};

struct input_t {
    // keyboard
    const uint8_t *keys_pressed; // this stores the state of each key and is always updated

    // mouse
    mouse_t mouse;
};

// --------------------
// Basic data
// --------------------

struct color_t {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
};

// ---------------------
// Game data
// ---------------------

// entity types
enum entity_type_e {
    
};

enum game_mode_e {
    PAUSED,
    PLAYING,
    START_SCREEN,
};

// enemy specific data
struct enemy_only_t {

};

// entity generic data 
struct entity_t {
    v2 pos;
    double speed;

    SDL_Texture *image;
    int w;
    int h;

    entity_type_e type;
    union {
        enemy_only_t enemy_data; // this should be more specific than enemy (it is just an example)
        // ... other entities data (i.e. shopkeeper specific data)
    };
};

enum tile_type_e {
    EMPTY,
    WALL,
};

struct map_t {
    int w, h;
    tile_type_e tile[30][40];

    SDL_Texture *wall_sprite;
    SDL_Texture *floor_sprite;
};

// game state data
struct game_state_t {
    // current game mode
    game_mode_e game_mode;

    // map
    map_t map;

    // scene data
    color_t background_color;
};
