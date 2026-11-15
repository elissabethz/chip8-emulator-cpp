#include <iostream>
#include <SDL2/SDL.h>
#include "chip8.hpp"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Uso: " << argv[0] << " <ruta_a_la_rom>" << std::endl;
        return -1;
    }

    Chip8 chip8;
    if (!chip8.loadROM(argv[1])) return -1;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Error al inicializar SDL: " << SDL_GetError() << std::endl;
        return -1;
    }

    const int SCALE = 10;
    SDL_Window* window = SDL_CreateWindow(
        "CHIP-8 Emulator",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        64 * SCALE, 32 * SCALE, SDL_WINDOW_SHOWN
    );

    if (!window) {
        SDL_Quit();
        return -1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    
    bool quit = false;
    SDL_Event e;

    // --- RELOJ MEJORADO (Desvinculado del delay del OS) ---
    uint32_t lastTimerTick = SDL_GetTicks();

    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) quit = true;
            
            if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_1: chip8.keypad[0x1] = 1; break;
                    case SDLK_2: chip8.keypad[0x2] = 1; break;
                    case SDLK_3: chip8.keypad[0x3] = 1; break;
                    case SDLK_4: chip8.keypad[0xC] = 1; break;
                    case SDLK_q: chip8.keypad[0x4] = 1; break;
                    case SDLK_w: chip8.keypad[0x5] = 1; break;
                    case SDLK_e: chip8.keypad[0x6] = 1; break;
                    case SDLK_r: chip8.keypad[0xD] = 1; break;
                    case SDLK_a: chip8.keypad[0x7] = 1; break;
                    case SDLK_s: chip8.keypad[0x8] = 1; break;
                    case SDLK_d: chip8.keypad[0x9] = 1; break;
                    case SDLK_f: chip8.keypad[0xE] = 1; break;
                    case SDLK_z: chip8.keypad[0xA] = 1; break;
                    case SDLK_x: chip8.keypad[0x0] = 1; break;
                    case SDLK_c: chip8.keypad[0xB] = 1; break;
                    case SDLK_v: chip8.keypad[0xF] = 1; break;
                }
            }
            if (e.type == SDL_KEYUP) {
                switch (e.key.keysym.sym) {
                    case SDLK_1: chip8.keypad[0x1] = 0; break;
                    case SDLK_2: chip8.keypad[0x2] = 0; break;
                    case SDLK_3: chip8.keypad[0x3] = 0; break;
                    case SDLK_4: chip8.keypad[0xC] = 0; break;
                    case SDLK_q: chip8.keypad[0x4] = 0; break;
                    case SDLK_w: chip8.keypad[0x5] = 0; break;
                    case SDLK_e: chip8.keypad[0x6] = 0; break;
                    case SDLK_r: chip8.keypad[0xD] = 0; break;
                    case SDLK_a: chip8.keypad[0x7] = 0; break;
                    case SDLK_s: chip8.keypad[0x8] = 0; break;
                    case SDLK_d: chip8.keypad[0x9] = 0; break;
                    case SDLK_f: chip8.keypad[0xE] = 0; break;
                    case SDLK_z: chip8.keypad[0xA] = 0; break;
                    case SDLK_x: chip8.keypad[0x0] = 0; break;
                    case SDLK_c: chip8.keypad[0xB] = 0; break;
                    case SDLK_v: chip8.keypad[0xF] = 0; break;
                }
            }
        }

        uint32_t currentTime = SDL_GetTicks();

        // Actualizamos la pantalla y los temporizadores exactamente a 60 hercios (~16ms)
        if (currentTime - lastTimerTick >= 16) {
            
            // 1. Ejecutamos 10 ciclos de CPU de golpe (~600 hercios totales)
            for (int i = 0; i < 2; ++i) {
                chip8.cycle();
            }

            // 2. Descontamos los temporizadores de sonido y retardo
            chip8.updateTimers();

            // 3. Lógica de dibujado
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

            for (int y = 0; y < 32; ++y) {
                for (int x = 0; x < 64; ++x) {
                    if (chip8.video[y * 64 + x] != 0) {
                        SDL_Rect rect = {x * SCALE, y * SCALE, SCALE, SCALE};
                        SDL_RenderFillRect(renderer, &rect);
                    }
                }
            }

            SDL_RenderPresent(renderer);
            chip8.drawFlag = false;
        }

        // Dejamos que el kernel de Linux descanse el hilo sin importar si se pasa un poco
        SDL_Delay(1); 
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}