#include <SDL2/SDL.h>
#include <ctime>
#include <cstdio>

const int scale = 4;
const uint8_t font[80] = {
        0x60, 0xa0, 0xa0, 0xa0, 0xc0,
        0x40, 0xc0, 0x40, 0x40, 0xe0,
        0xc0, 0x20, 0x40, 0x80, 0xe0,
        0xc0, 0x20, 0x40, 0x20, 0xc0,
        0x20, 0xa0, 0xe0, 0x20, 0x20,
        0xe0, 0x80, 0xc0, 0x20, 0xc0,
        0x40, 0x80, 0xc0, 0xa0, 0x40,
        0xe0, 0x20, 0x60, 0x40, 0x40,
        0x40, 0xa0, 0x40, 0xa0, 0x40,
        0x40, 0xa0, 0x60, 0x20, 0x40,
        0x40, 0xa0, 0xe0, 0xa0, 0xa0,
        0xc0, 0xa0, 0xc0, 0xa0, 0xc0,
        0x60, 0x80, 0x80, 0x80, 0x60,
        0xc0, 0xa0, 0xa0, 0xa0, 0xc0,
        0xe0, 0x80, 0xc0, 0x80, 0xe0,
        0xe0, 0x80, 0xc0, 0x80, 0x80
};

uint32_t screen[64*32];
uint8_t memory[0x1000];
uint8_t v[16];
uint16_t pc = 0x200;
uint16_t idx = 0x0000;
uint16_t stack[24];
uint8_t stackPointer = 0;
uint8_t delayTimer = 0;
uint8_t soundTimer = 0;
uint8_t keys[16];

void push(uint16_t valor){
    stack[stackPointer++] = valor;
}
uint16_t pop(){
    return stack[--stackPointer];
}
void setPixel(uint8_t x, uint8_t y, uint32_t value) {
    screen[ y * 64 + x ] = value;
}
uint32_t getPixel(uint8_t x, uint8_t y) {
    return screen[ y * 64 + x ];
}
void avanzarEmulacion() {
    // Fetch
    uint16_t opcode = (memory[pc]<<8) | memory[pc+1];
    pc += 2;
    // Decode
    uint8_t nibble1 = opcode >> 12;
    uint8_t nibble2 = (opcode >> 8) & 0xF;
    uint8_t nibble3 = (opcode >> 4) & 0xF;
    uint8_t nibble4 = opcode & 0xF;
    uint16_t address = opcode & 0xFFF;
    uint8_t byte2 = opcode & 0xFF;
    // Execute
    if(nibble1 == 0x0){
        if(address == 0x0E0){
            for(int i=0;i<64*32;i++) screen[i] = 0;
        } else if (address == 0x0EE){
            pc = pop();
        }
    } else if(nibble1 == 0x1) {
        pc = address;
    } else if(nibble1 == 0x2){
        push(pc);
        pc = address;
    } else if(nibble1 == 0x3){
        if(v[nibble2] == byte2){
            pc += 2;
        }
    } else if(nibble1 == 0x4){
        if(v[nibble2] != byte2){
            pc += 2;
        }
    } else if(nibble1 == 0x5){
        if(v[nibble2] == v[nibble3]){
            pc += 2;
        }
    } else if(nibble1 == 0x6){
        v[nibble2] = byte2;
    } else if(nibble1 == 0x7){
        v[nibble2] += byte2;
    } else if(nibble1 == 0x8){
        if(nibble4 == 0x0) {
            v[nibble2] = v[nibble3];
        } else if(nibble4 == 0x1){
            v[nibble2] |= v[nibble3];
            v[0xF] = 0; // Quirk
        } else if(nibble4 == 0x2){
            v[nibble2] &= v[nibble3];
            v[0xF] = 0; // Quirk
        } else if(nibble4 == 0x3){
            v[nibble2] ^= v[nibble3];
            v[0xF] = 0; // Quirk
        } else if(nibble4 == 0x4){
            uint16_t sum = v[nibble2] + v[nibble3];
            v[nibble2] += v[nibble3];
            v[0xF] = sum > 0xFF ? 1 : 0;
        } else if(nibble4 == 0x5){
            int borrow = v[nibble2] >= v[nibble3] ? 1 : 0;
            v[nibble2] -= v[nibble3];
            v[0xF] = borrow;
        } else if(nibble4 == 0x6){
            //v[nibble2] = v[nibble3];
            int lsb = v[nibble2] & 1;
            v[nibble2] >>= 1;
            v[0xF] = lsb;
        } else if(nibble4 == 0x7){
            int borrow = v[nibble3] >= v[nibble2] ? 1 : 0;
            v[nibble2] = v[nibble3] - v[nibble2];
            v[0xF] = borrow;
        } else if(nibble4 == 0xE){
            //v[nibble2] = v[nibble3];
            int msb = v[nibble2] >> 7;
            v[nibble2] <<= 1;
            v[0xF] = msb;
        }
    } else if(nibble1 == 0x9 && nibble4 == 0x0) {
        if(v[nibble2] != v[nibble3]) pc+=2;
    } else if(nibble1 == 0xA) {
        idx = address;
    } else if(nibble1 == 0xB) {
        pc = v[0] + address;
    } else if(nibble1 == 0xC) {
        v[nibble2] = rand() & byte2;
    }else if(nibble1 == 0xD) {
        v[0xF] = 0;
        int x0 = v[nibble2]%64;
        int y0 = v[nibble3]%32;
        for (int y=0;y<nibble4;y++) {
            uint8_t actual = memory[idx + y];
            for (int x = 0; x < 8; x++) {
                int rx = x + x0;
                int ry = y + y0;
                if ((actual & (0x80)) != 0 && rx < 64 && ry < 32) {
                    if (getPixel(rx, ry)) {
                        setPixel(rx, ry, 0);
                        v[0xF] = 1;
                    } else {
                        setPixel(rx, ry, 0xFFFFFF);
                    }
                }
                actual <<= 1;
            }
        }
    } else if(nibble1 == 0xE) {
        if(byte2 == 0x9E){
            if(keys[v[nibble2]]) pc += 2;
        }else if(byte2 == 0xA1){
            if(!keys[v[nibble2]]) pc += 2;
        }
    } else if(nibble1 == 0xF) {
        if(byte2 == 0x07){
            v[nibble2] = delayTimer;
        }else if(byte2 == 0x0A){
            bool ningunaTecla = true;
            int tecla = 0;
            for(int i=0;i<16;i++) if(keys[i]) { ningunaTecla = false; tecla = i; }
            if(ningunaTecla) pc -= 2; else v[nibble2] = tecla;
        }else if(byte2 == 0x15) {
            delayTimer = v[nibble2];
        }else if(byte2 == 0x18) {
            soundTimer = v[nibble2];
        }else if(byte2 == 0x1E) {
            idx += v[nibble2];
        }else if(byte2 == 0x29) {
            idx = v[nibble2] * 5;
        }else if(byte2 == 0x33){
            memory[idx] = v[nibble2]/100;
            memory[idx+1] = (v[nibble2]/10)%10;
            memory[idx+2] = v[nibble2]%10;
        }else if(byte2 == 0x55){
            for(int i=0;i<=nibble2;i++) memory[idx+i] = v[i];
            idx+=nibble2+1;
        }else if(byte2 == 0x65){
            for(int i=0;i<=nibble2;i++) v[i] = memory[idx+i];
            idx+=nibble2+1;
        }
    }
}
int main(int argc, char **args){
    SDL_Event event;
    SDL_Renderer *renderer;
    SDL_Window *window;
    srand(time(nullptr));

    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(64*scale, 32*scale, 0, &window, &renderer);
    SDL_Texture *texture = SDL_CreateTexture(renderer,SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, 64, 32);
    for(int i=0;i<80;i++) memory[i] = font[i];

    FILE *input = fopen("3-corax+.ch8", "rb");
    fread(&memory[0x200], 0xE00, 1, input);
    fclose(input);

    bool fin = false;
    while (!fin) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                fin = true;
            } else if(event.type == SDL_KEYDOWN){
                if(event.key.keysym.sym == SDLK_1) keys[1] = 1;
                if(event.key.keysym.sym == SDLK_2) keys[2] = 1;
                if(event.key.keysym.sym == SDLK_3) keys[3] = 1;
                if(event.key.keysym.sym == SDLK_4) keys[0xC] = 1;
                if(event.key.keysym.sym == SDLK_q) keys[4] = 1;
                if(event.key.keysym.sym == SDLK_w) keys[5] = 1;
                if(event.key.keysym.sym == SDLK_e) keys[6] = 1;
                if(event.key.keysym.sym == SDLK_r) keys[0xD] = 1;
                if(event.key.keysym.sym == SDLK_a) keys[7] = 1;
                if(event.key.keysym.sym == SDLK_s) keys[8] = 1;
                if(event.key.keysym.sym == SDLK_d) keys[9] = 1;
                if(event.key.keysym.sym == SDLK_f) keys[0xE] = 1;
                if(event.key.keysym.sym == SDLK_z) keys[0xA] = 1;
                if(event.key.keysym.sym == SDLK_x) keys[0] = 1;
                if(event.key.keysym.sym == SDLK_c) keys[0xB] = 1;
                if(event.key.keysym.sym == SDLK_v) keys[0xF] = 1;
            } else if(event.type == SDL_KEYUP){
                if(event.key.keysym.sym == SDLK_1) keys[1] = 0;
                if(event.key.keysym.sym == SDLK_2) keys[2] = 0;
                if(event.key.keysym.sym == SDLK_3) keys[3] = 0;
                if(event.key.keysym.sym == SDLK_4) keys[0xC] = 0;
                if(event.key.keysym.sym == SDLK_q) keys[4] = 0;
                if(event.key.keysym.sym == SDLK_w) keys[5] = 0;
                if(event.key.keysym.sym == SDLK_e) keys[6] = 0;
                if(event.key.keysym.sym == SDLK_r) keys[0xD] = 0;
                if(event.key.keysym.sym == SDLK_a) keys[7] = 0;
                if(event.key.keysym.sym == SDLK_s) keys[8] = 0;
                if(event.key.keysym.sym == SDLK_d) keys[9] = 0;
                if(event.key.keysym.sym == SDLK_f) keys[0xE] = 0;
                if(event.key.keysym.sym == SDLK_z) keys[0xA] = 0;
                if(event.key.keysym.sym == SDLK_x) keys[0] = 0;
                if(event.key.keysym.sym == SDLK_c) keys[0xB] = 0;
                if(event.key.keysym.sym == SDLK_v) keys[0xF] = 0;
            }
        }
        if(delayTimer > 0) delayTimer--;
        if(soundTimer > 0) soundTimer--;
        for(int i=0;i<8;i++) avanzarEmulacion();
        // Dibujar pantalla
        SDL_UpdateTexture(texture, NULL, screen, 64 * sizeof(Uint32));
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
