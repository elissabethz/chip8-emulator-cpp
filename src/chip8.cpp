#include "chip8.hpp"
#include <fstream>
#include <iostream>

Chip8::Chip8() {
    //El Program Counter siempre empieza en 0x200 (512 en decimal)
    pc = 0x200;
}

bool Chip8::loadROM(const char* filename) {

    //Abrir el archivo en modo binario y poner el cursor al final (ate) para saber el tamaño
    std::ifstream file(filename, std::ios::binary | std::ios::ate);

    if(file.is_open()) {

        //Obtener el tamaño del archivo y volver al principio
        std::streampos size = file.tellg();
        char* buffer = new char[size];
        file.seekg(0, std::ios::beg);
        file.read(buffer, size);
        file.close();

        //Volcar el contenido del archivo en la memoria de la maquina (empezando en 0x200)
        for(long i = 0; i < size; ++i) {
            memory[0x200 + i] = buffer[i];
        }

        delete[] buffer;
        std::cout << "ROM cargada en memoria correctamente: " << size << " bytes." << std::endl;
        return true;
    } else {
        std::cout << "Error: No se pudo abrir el archivo ROM: " << filename << std::endl;
        return false;
    }
}

void Chip8::cycle() {

    //Seguridad: Si el PC llega al limite de la RAM, evitamos la violacion de segmento
    if (pc >= 4090) {
        return;
    }

    //FETCH: Leer 2 bytes de la memoria y unirlos en un opcode de 16 bits
    opcode = (memory[pc] << 8u) | memory[pc + 1];

    //Incrementar el Program Counter en 2 para apuntar a la siguiente instruccion
    pc += 2;

    //DECODE: Extraemos los parametros comunes que usan las instruciones de CHIP-8
    uint8_t X = (opcode & 0x0F00u) >> 8u;       //Extrae el '2' (Registro Vx)
    uint8_t Y = (opcode & 0x00F0u) >> 4u;       //Extrae el '2' (Registro Vy)
    uint8_t N = opcode & 0x000Fu;               //Extrae el 'A' (Constante 4-bits)
    uint8_t NN = opcode & 0x00FFu;              //Extrae el '2A' (Constante 8-bits)
    uint16_t NNN = opcode & 0x0FFFu;            //Extrae el '22A' (Direccion de memoria 12-bits)

    //EXECUTE: Miramos solo el primer digito hexadecimal (los 4 bits más significativos)
    switch (opcode & 0xF000u) {

        case 0x0000:
            if (opcode == 0x00E0) {
                //00E0: Limpiar la pantalla (Llenar el array de video con ceros)
                memset(video, 0, sizeof(video));
            }
            break;

        case 0x1000:
            //Instrucion 1NNN: Salto (JUMP)
            //Cambia el Program Counter a la direccion NNN
            pc = NNN;
            break;
        
        case 0x6000:
            //6XNN: Establecer Vx = NN
            registers[X] = NN;
            break;

        case 0x7000:
            //7XNN: Sumar NN a Vx
            registers[X] += NN;
            break;

        case 0xA000:
            //Instrucion ANNN: Establecer indice
            //El registro 'index' (I) toma el valor NNN
            index = NNN;
            break;

        case 0xD000:
            //DXYN: Dibujar sprite en pantalla (XOR)
            uint8_t xPos = registers[X] % 64;
            uint8_t yPos = registers[Y] % 32;
            registers[0xF] = 0;             //Resetear registro de colision

            //Iterar sobre las filas del sprite
            for (unsigned int row = 0; row < N; ++row) {
                uint8_t spriteByte = memory[index + row];

                //Iterar sobre los 8 bits (columnas) de la fila actual
                for (unsigned int col = 0; col < 8; ++col) {
                    uint8_t
                }
            }
    }
}