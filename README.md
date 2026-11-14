# CHIP-8 Emulator

A cross-platform CHIP-8 virtual machine written in modern C++ and SDL2. 

This project implements the complete Fetch-Decode-Execute cycle, simulating the original 4KB RAM, 16 8-bit registers, and a 64x32 monochrome display.

## Overview
*   **Language:** C++
*   **Graphics:** SDL2
*   **Build System:** Make
*   **Platform:** Linux (Ubuntu) / Windows (via MSYS2/MinGW)

## Build Instructions (Linux / Ubuntu)

1. **Install dependencies:**
   ```bash
   sudo apt update
   sudo apt install libsdl2-dev
