#include "chip8.hpp"
#include <fstream>
#include <iostream>
#include <cstring>
#include <cstdlib> 
#include <ctime>   

const unsigned int FONTSET_SIZE = 80;
const uint8_t fontset[FONTSET_SIZE] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

Chip8::Chip8() {
    pc = 0x200;
    srand(time(NULL)); 
    for (unsigned int i = 0; i < FONTSET_SIZE; ++i) {
        memory[0x50 + i] = fontset[i];
    }
}

bool Chip8::loadROM(const char* filename) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (file.is_open()) {
        std::streampos size = file.tellg();
        char* buffer = new char[size];
        file.seekg(0, std::ios::beg);
        file.read(buffer, size);
        file.close();
        for (long i = 0; i < size; ++i) {
            memory[0x200 + i] = buffer[i];
        }
        delete[] buffer;
        std::cout << "ROM cargada: " << size << " bytes." << std::endl;
        return true;
    }
    std::cerr << "Error: No se pudo abrir " << filename << std::endl;
    return false;
}

void Chip8::updateTimers() {
    if (delayTimer > 0) --delayTimer;
    if (soundTimer > 0) {
        if (soundTimer == 1) std::cout << "BEEP!" << std::endl;
        --soundTimer;
    }
}

void Chip8::cycle() {
    if (pc >= 4094) return;

    opcode = (memory[pc] << 8u) | memory[pc + 1];
    pc += 2;

    uint8_t X   = (opcode & 0x0F00u) >> 8u;
    uint8_t Y   = (opcode & 0x00F0u) >> 4u;
    uint8_t N   = opcode & 0x000Fu;
    uint8_t NN  = opcode & 0x00FFu;
    uint16_t NNN = opcode & 0x0FFFu;

    switch (opcode & 0xF000u) {
        case 0x0000:
            if (opcode == 0x00E0) {
                memset(video, 0, sizeof(video));
            } else if (opcode == 0x00EE) {
                --sp;
                pc = stack[sp]; 
            }
            break;
        case 0x1000: pc = NNN; break;
        case 0x2000: stack[sp] = pc; ++sp; pc = NNN; break; 
        case 0x3000: if (registers[X] == NN) pc += 2; break;
        case 0x4000: if (registers[X] != NN) pc += 2; break;
        case 0x5000: if (registers[X] == registers[Y]) pc += 2; break;
        case 0x6000: registers[X] = NN; break;
        case 0x7000: registers[X] += NN; break;
        case 0x8000:
            switch (N) {
                case 0x0: registers[X]  = registers[Y]; break;
                case 0x1: registers[X] |= registers[Y]; break;
                case 0x2: registers[X] &= registers[Y]; break;
                case 0x3: registers[X] ^= registers[Y]; break;
                case 0x4: {
                    uint16_t sum = registers[X] + registers[Y];
                    registers[0xF] = (sum > 255U) ? 1 : 0;
                    registers[X] = sum & 0xFFu;
                } break;
                case 0x5:
                    registers[0xF] = (registers[X] >= registers[Y]) ? 1 : 0;
                    registers[X] -= registers[Y];
                    break;
                case 0x6:
                    registers[0xF] = (registers[X] & 0x1);
                    registers[X] >>= 1;
                    break;
                case 0x7:
                    registers[0xF] = (registers[Y] >= registers[X]) ? 1 : 0;
                    registers[X] = registers[Y] - registers[X];
                    break;
                case 0xE:
                    registers[0xF] = (registers[X] & 0x80) >> 7;
                    registers[X] <<= 1;
                    break;
            }
            break;
        case 0x9000: if (registers[X] != registers[Y]) pc += 2; break;
        case 0xA000: index = NNN; break;
        case 0xB000: pc = NNN + registers[0]; break;
        case 0xC000: registers[X] = (rand() % 256) & NN; break;
        
        // --- LA MAGIA ESTÁ AQUÍ (Instrucción de dibujado protegida) ---
        case 0xD000: {
            uint8_t xPos = registers[X] % 64;
            uint8_t yPos = registers[Y] % 32;
            registers[0xF] = 0;
            
            for (unsigned int row = 0; row < N; ++row) {
                // SEGURIDAD: Si el píxel se sale por debajo del límite, cortamos.
                if (yPos + row >= 32) break;
                
                uint8_t spriteByte = memory[index + row];
                for (unsigned int col = 0; col < 8; ++col) {
                    // SEGURIDAD: Si el píxel se sale por la derecha, cortamos.
                    if (xPos + col >= 64) break;
                    
                    uint8_t spritePixel = spriteByte & (0x80u >> col);
                    if (spritePixel) {
                        uint32_t* screenPixel = &video[(yPos + row) * 64 + (xPos + col)];
                        if (*screenPixel == 0xFFFFFFFF) registers[0xF] = 1;
                        *screenPixel ^= 0xFFFFFFFF;
                    }
                }
            }
            drawFlag = true;
        } break;
        // ---------------------------------------------------------------

        case 0xE000:
            if (NN == 0x9E) { if (keypad[registers[X]]) pc += 2; }
            else if (NN == 0xA1) { if (!keypad[registers[X]]) pc += 2; }
            break;
        case 0xF000:
            switch (NN) {
                case 0x07: registers[X] = delayTimer; break;
                case 0x0A: {
                    bool keyPress = false;
                    for (int i = 0; i < 16; ++i) {
                        if (keypad[i]) { registers[X] = i; keyPress = true; }
                    }
                    if (!keyPress) pc -= 2; 
                } break;
                case 0x15: delayTimer = registers[X]; break;
                case 0x18: soundTimer = registers[X]; break;
                case 0x1E: index += registers[X]; break;
                case 0x29: index = 0x50 + (5 * registers[X]); break;
                case 0x33:
                    memory[index]     = registers[X] / 100;
                    memory[index + 1] = (registers[X] / 10) % 10;
                    memory[index + 2] = registers[X] % 10;
                    break;
                case 0x55:
                    for (uint8_t i = 0; i <= X; ++i) memory[index + i] = registers[i];
                    break;
                case 0x65:
                    for (uint8_t i = 0; i <= X; ++i) registers[i] = memory[index + i];
                    break;
            }
            break;
    }
}