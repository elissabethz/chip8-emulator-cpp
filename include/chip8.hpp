#pragma once
#include <cstdint>

class Chip8 {
    public:
        Chip8();                                //Constructor
        bool loadROM(const char* filename);     //Nueva funcion para cargar juegos
        void cycle();
        void updateTimers();

        //Elementos publicos que leera el main (la pantalla y el teclado)
        uint8_t keypad[16]{};                   //Teclado hexadecimal (0-F)
        uint32_t video[64 * 32]{};              //Pantalla monocromo de 64x32 pixeles
        bool drawFlag{};

    private:
        //Componentes internos de la CPU
        uint8_t registers[16]{};                //16 registros de 8 bits (V0 a VF)
        uint8_t memory[4096]{};                 //4KB de memoria RAM
        uint16_t index{};                       //Registro Indice (I) de 16 bits
        uint16_t pc{};                          //Program Counter (Apunta a la instruccion actual)
        uint16_t stack[16]{};                   //Pila para llamadas a funciones (subrutinas)
        uint8_t sp{};                           //Stack Pointer (Apunta al tope de la pila)
        uint8_t delayTimer{};                   //Temporizador de retardo
        uint8_t soundTimer{};                   //Temporizador de sonido
        uint16_t opcode{};                      //La instruccion actual a ejecutar
};