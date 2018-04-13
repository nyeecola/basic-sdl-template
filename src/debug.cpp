static TTF_Font *global_debug_font = 0;
static SDL_Texture *global_glyphs[256] = {};
static SDL_Renderer *global_debug_renderer;

void create_texture_for_glyphs(int index) {
    char str[2] = {(char) index, 0};

    // TODO: allow different font colors
    SDL_Color white;
    white.r = 255;
    white.g = 255;
    white.b = 255;
    white.a = 255;

    SDL_Surface *surface = 0;
    if (!(surface = TTF_RenderText_Solid(global_debug_font, str, white))) {
        puts("Failed to create surface from text.");
        printf("Error: %s\n", TTF_GetError());
        return;
    }

    SDL_Texture *texture = 0;
    if (!(texture = SDL_CreateTextureFromSurface(global_debug_renderer, surface))) {
        puts("Failed to create texture from surface.");
        printf("Error: %s\n", TTF_GetError());
        return;
    }

    global_glyphs[index] = texture;

    SDL_FreeSurface(surface);
}

// Creates glyphs
void debug_initialize_text(SDL_Renderer *renderer) {
    global_debug_renderer = renderer;

    if (TTF_Init() == -1) {
        puts("Failed to initialize TTF.");
        printf("Error: %s\n", TTF_GetError());
        return;
    }

    if (!(global_debug_font = TTF_OpenFont(FONT_PATH, FONT_SIZE))) {
        puts("Failed to open font file.");
        printf("Error: %s\n", TTF_GetError());
        return;
    }

    // create a texture for each glyph
    for (int i = 0; i < 10; i++) {
        create_texture_for_glyphs('0' + i);
    }
    for (int i = 0; i < 26; i++) {
        create_texture_for_glyphs('a' + i);
    }
    for (int i = 0; i < 26; i++) {
        create_texture_for_glyphs('A' + i);
    }
    create_texture_for_glyphs(' ');
    create_texture_for_glyphs('-');
    create_texture_for_glyphs(':');
    create_texture_for_glyphs(';');
    create_texture_for_glyphs(',');
    create_texture_for_glyphs('.');
    create_texture_for_glyphs('!');
    create_texture_for_glyphs('?');
    create_texture_for_glyphs('(');
    create_texture_for_glyphs(')');
    create_texture_for_glyphs('{');
    create_texture_for_glyphs('}');
    create_texture_for_glyphs('[');
    create_texture_for_glyphs(']');
    create_texture_for_glyphs('+');
    create_texture_for_glyphs('-');
    create_texture_for_glyphs('*');
    create_texture_for_glyphs('/');
    create_texture_for_glyphs('\\');
    create_texture_for_glyphs('&');
    create_texture_for_glyphs('#');
    create_texture_for_glyphs('@');
    create_texture_for_glyphs('"');
    create_texture_for_glyphs('\'');
    create_texture_for_glyphs('%');
    create_texture_for_glyphs('$');
    create_texture_for_glyphs('=');
    create_texture_for_glyphs('_');
}

// Draws text on x,y coordinates.
// FONT_SIZE is multiplied by the scale.
void debug_draw_text(const char *str, int x, int y, float scale) {
    int len = strlen(str);
    for (int i = 0; i < len; i++) {
        SDL_Texture *glyph = global_glyphs[(int) str[i]];
        SDL_Rect rect;

        if(SDL_QueryTexture(glyph, 0, 0, &rect.w, &rect.h) < 0) {
            printf("Error: %s\n", SDL_GetError());
            return;
        }

        rect.w *= scale;
        rect.h *= scale;
        rect.x = x + rect.w * i;
        rect.y = y;

        if(SDL_RenderCopy(global_debug_renderer, glyph, 0, &rect) < 0) {
            printf("Error: %s\n", SDL_GetError());
            return;
        }
    }
}

// Draws the average fps on the screen
// NOTE: this function must be called exactly once every frame in order to give
//       correct results
// TODO: check FPS, is probably wrong
void debug_draw_fps(double dt) {
    static double timer = 0;
    static int counter = 0;
    static char fps_text[15] = "";
    timer += dt;
    counter++;
    if (timer > 80) {
        snprintf(&fps_text[0], 15, "FPS: %.00f", 1 / (timer / (1000 * counter)));
        timer = 0;
        counter = 0;
    }
    debug_draw_text(&fps_text[0], 0, 0, 1);
}
