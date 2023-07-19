#include <SDL2/SDL.h>
#include <stdint.h>

const int scale = 4;
const int width = 64;
const int height = 32;
uint32_t screen[width * height];

void setPixel(uint8_t x, uint8_t y, uint32_t value) {
    screen[y * width + x] = value;
}

uint32_t getPixel(uint8_t x, uint8_t y) {
    return screen[y * width + x];
}

int main(int argc, char **args){
    SDL_Event event;
    SDL_Renderer *renderer;
    SDL_Window *window;

    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(width*scale, height*scale, 0, &window, &renderer);
    SDL_Texture *texture = SDL_CreateTexture(renderer,SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, width, height);
    
    bool quit = false;
    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            } else if(event.type == SDL_KEYDOWN){
                if(event.key.keysym.sym == SDLK_1) setPixel(10, 10, 0xFFFFFFFF);
            } else if(event.type == SDL_KEYUP){
                if(event.key.keysym.sym == SDLK_1) setPixel(10, 10, 0x00000000);
            }
        }

        
        // Dibujar pantalla
        SDL_UpdateTexture(texture, NULL, screen, width * sizeof(Uint32));
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
        // Esperar al próximo frame
        SDL_Delay(16);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
