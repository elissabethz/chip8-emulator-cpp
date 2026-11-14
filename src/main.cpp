#include <iostream>
#include <SDL2/SDL.h>
#include "chip8.hpp"

int main(int argc, char* argv[]) {

    //1. Comprobar que pasamos la ROM por argumento
    if (argc != 2) {
        std::cerr << "Uso: " << argv[0] << " <ruta_a_la_rom>" << std::endl;
        return -1;
    }

    //2. Crear la maquina virtual
    Chip8 chip8;
    if (!chip8.loadROM(argv[1])) {
        //Si falla la carga, abortamos y salimos del main
        return -1;
    }

    //3. Inicializar el subsistema de video de SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Error al inicializar SDL:" << SDL_GetError() << std::endl;
        return -1;
    }

    //4. Crear la ventana (Resolución original 64x32, escala x10)
    const int SCALE = 10;
    SDL_Window* window = SDL_CreateWindow(
        "CHIP-8 Emulador",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        64 * SCALE,
        32 * SCALE,
        SDL_WINDOW_SHOWN
    );

    if (!window) {
        std::cerr << "Error al crear la ventana:" << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    //Crear el renderizador (el motor que dibuja en la ventana)
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    //Pintar el fondo de negro puro (RGBA: 0, 0, 0, 255)
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

    std::cout << "¡Entorno gráfico inicializado correctamente!" << std::endl;

    //5. BUcle principal del emulador (Game Loop)
    bool quit = false;
    SDL_Event e;

    while (!quit) {

        //Procesar eventos de la ventana (escuchar si pulsas la 'X' para cerrar)
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }

        //Ejecutar un ciclo de la CPU
        chip8.cycle();

        //Pausa de 1ms para no saturar la CPU de tu ordenador
        SDL_Delay(1);
    }

    //Limpiar la memoria y cerrar limpiamente
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}