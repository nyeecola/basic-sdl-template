void force_quit(const char *str)
{
    puts(str);
    printf("Error: %s\n", SDL_GetError());
    SDL_Quit();
    exit(1);
}

input_t *initialize_input()
{
    input_t *input = (input_t *) malloc(sizeof(*input));

    input->keys_pressed = SDL_GetKeyboardState(0);
    input->mouse.x = 0;
    input->mouse.y = 0;

    return input;
}

// verifies if image was already renderer
// if not, render and display it
// if yes, just display it
// TODO: verify if this is too slow (because of the conversion to std::string every call
void display_image(renderer_t *renderer, const char *char_path, SDL_Rect *dest, SDL_Rect *source, v3 color)
{
    std::string path(char_path);
    if (!renderer->images.count(path))
    {
        SDL_Texture *tex = IMG_LoadTexture(renderer->sdl, char_path);
        renderer->images[path] = tex;
    }

    SDL_SetTextureColorMod(renderer->images[char_path], (u8) color.x, (u8) color.y, (u8) color.z);
    SDL_RenderCopy(renderer->sdl, renderer->images[char_path], source, dest);
}

