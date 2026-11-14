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