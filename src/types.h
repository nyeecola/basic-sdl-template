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
    ENEMY,
    PLAYER,
};

enum game_mode_e {
    PAUSED,
    PLAYING,
    START_SCREEN,
};

// enemy specific data
struct enemy_t {
    v2 destination;
};

// entity generic data 
struct entity_t {
    v2 previous_pos; // used to calculate rotation
    v2 pos;
    double speed;
    double angle;

    SDL_Texture *image;
    int image_w;
    int image_h;

    int hitbox_r;

    entity_type_e type;
    union {
        enemy_t enemy_data; // this should be more specific than enemy (it is just an example)
        // ... other entities data (i.e. shopkeeper specific data)
    };
};

enum tile_type_e {
    EMPTY,
    WALL,
    DOOR
};

struct door_t {
    int x;
    int y;
    int exit_x;
    int exit_y;
    int target_map;
    int target_door;
};

struct map_t {
    int w, h;
    int doors;

    door_t door[MAX_DOOR_PER_ROOM];
    tile_type_e tile[30][40];

    SDL_Texture *wall_sprite;
    SDL_Texture *floor_sprite;
    SDL_Texture *door_sprite;
};

// game state data
struct game_state_t {
    // current game mode
    game_mode_e game_mode;

    // map
    int current_map_id;
    map_t *map;

    //npcs
    entity_t enemies[128];
    int enemies_count;

    // player
    entity_t player;

    // scene data
    color_t background_color;
};

