#include "Chip8.hpp"
#include "Memory.hpp"
#include "Display.hpp"
#include "Keyboard.hpp"
#include <fstream>
#include <chrono>
#include <cstring>
#include <iostream>

namespace Chip8Emu {

// Font data (5 bytes per character, 16 characters)
const uint8_t FONT_DATA[80] = {
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

// Program start address in memory
constexpr uint16_t PROGRAM_START_ADDRESS = 0x200;
// Font data start address in memory
constexpr uint16_t FONT_START_ADDRESS = 0x50;

Chip8::Chip8()
    : I(0)
    , pc(PROGRAM_START_ADDRESS)
    , sp(0)
    , delayTimer(0)
    , soundTimer(0)
    , rng(std::random_device{}())
    , randByte(0, 255)
    , beepCallback(nullptr)
{
    memory = std::make_unique<Memory>();
    display = std::make_unique<Display>();
    keyboard = std::make_unique<Keyboard>();
    
    initialize();
}

Chip8::~Chip8() = default;

void Chip8::initialize() {
    // Clear memory, registers, and display
    memory->clear();
    display->clear();
    keyboard->reset();
    
    // Reset registers
    V.fill(0);
    I = 0;
    pc = PROGRAM_START_ADDRESS;
    sp = 0;
    
    // Reset timers
    delayTimer = 0;
    soundTimer = 0;
    
    // Clear stack
    stack.fill(0);
    
    // Load fonts into memory
    loadFont();
}

void Chip8::loadFont() {
    memory->load(FONT_START_ADDRESS, FONT_DATA, sizeof(FONT_DATA));
}

bool Chip8::loadROM(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    
    if (!file.is_open()) {
        std::cerr << "Failed to open ROM file: " << filename << std::endl;
        return false;
    }
    
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    
    std::vector<uint8_t> buffer(size);
    if (!file.read(reinterpret_cast<char*>(buffer.data()), size)) {
        std::cerr << "Failed to read ROM file" << std::endl;
        return false;
    }
    
    return loadROM(buffer.data(), buffer.size());
}

bool Chip8::loadROM(const uint8_t* data, size_t size) {
    if (size > 4096 - PROGRAM_START_ADDRESS) {
        std::cerr << "ROM too large" << std::endl;
        return false;
    }
    
    // Reset emulator state
    initialize();
    
    // Load ROM into memory starting at PROGRAM_START_ADDRESS
    memory->load(PROGRAM_START_ADDRESS, data, size);
    
    return true;
}

void Chip8::reset() {
    initialize();
}

void Chip8::updateTimers() {
    if (delayTimer > 0) {
        delayTimer--;
    }
    
    if (soundTimer > 0) {
        soundTimer--;
        
        if (soundTimer == 0 && beepCallback) {
            beepCallback(false);  // Stop beeping
        } else if (beepCallback) {
            beepCallback(true);   // Start beeping
        }
    }
}

void Chip8::cycle() {
    // Fetch opcode (2 bytes)
    uint16_t opcode = (memory->read(pc) << 8) | memory->read(pc + 1);
    
    // Increment PC before execution
    pc += 2;
    
    // Execute the opcode
    executeOpcode(opcode);
}

void Chip8::executeOpcode(uint16_t opcode) {
    // Extract common opcode components
    uint8_t x = (opcode & 0x0F00) >> 8;    // The second nibble
    uint8_t y = (opcode & 0x00F0) >> 4;    // The third nibble
    uint8_t n = opcode & 0x000F;           // The fourth nibble
    uint8_t nn = opcode & 0x00FF;          // The second byte
    uint16_t nnn = opcode & 0x0FFF;        // The second, third and fourth nibbles

    // Decode and execute opcode
    switch (opcode & 0xF000) {
        case 0x0000:
            switch (opcode) {
                case 0x00E0: // 00E0: Clear the display
                    display->clear();
                    break;
                    
                case 0x00EE: // 00EE: Return from a subroutine
                    if (sp > 0) {
                        sp--;
                        pc = stack[sp];
                    }
                    break;
                    
                default:
                    std::cerr << "Unknown opcode: " << std::hex << opcode << std::endl;
                    break;
            }
            break;
            
        case 0x1000: // 1NNN: Jump to address NNN
            pc = nnn;
            break;
            
        case 0x2000: // 2NNN: Call subroutine at NNN
            if (sp < 16) {
                stack[sp] = pc;
                sp++;
                pc = nnn;
            }
            break;
            
        case 0x3000: // 3XNN: Skip next instruction if VX equals NN
            if (V[x] == nn) {
                pc += 2;
            }
            break;
            
        case 0x4000: // 4XNN: Skip next instruction if VX doesn't equal NN
            if (V[x] != nn) {
                pc += 2;
            }
            break;
            
        case 0x5000: // 5XY0: Skip next instruction if VX equals VY
            if (n == 0 && V[x] == V[y]) {
                pc += 2;
            }
            break;
            
        case 0x6000: // 6XNN: Set VX to NN
            V[x] = nn;
            break;
            
        case 0x7000: // 7XNN: Add NN to VX (no carry flag change)
            V[x] += nn;
            break;
            
        case 0x8000:
            switch (n) {
                case 0x0: // 8XY0: Set VX to the value of VY
                    V[x] = V[y];
                    break;
                    
                case 0x1: // 8XY1: Set VX to VX OR VY
                    V[x] |= V[y];
                    break;
                    
                case 0x2: // 8XY2: Set VX to VX AND VY
                    V[x] &= V[y];
                    break;
                    
                case 0x3: // 8XY3: Set VX to VX XOR VY
                    V[x] ^= V[y];
                    break;
                    
                case 0x4: // 8XY4: Add VY to VX, set VF to carry
                {
                    uint16_t sum = V[x] + V[y];
                    V[0xF] = (sum > 0xFF) ? 1 : 0; // Set carry flag
                    V[x] = sum & 0xFF;
                    break;
                }
                
                case 0x5: // 8XY5: Subtract VY from VX, set VF to NOT borrow
                {
                    V[0xF] = (V[x] >= V[y]) ? 1 : 0; // Set NOT borrow flag
                    V[x] -= V[y];
                    break;
                }
                
                case 0x6: // 8XY6: Shift VX right by 1, VF = LSB before shift
                    if (!config.shiftQuirks) {
                        // Original behavior: VX = VY then shift
                        V[x] = V[y];
                    }
                    V[0xF] = V[x] & 0x1; // Set VF to least significant bit
                    V[x] >>= 1;
                    break;
                    
                case 0x7: // 8XY7: Set VX to VY minus VX, set VF to NOT borrow
                {
                    V[0xF] = (V[y] >= V[x]) ? 1 : 0; // Set NOT borrow flag
                    V[x] = V[y] - V[x];
                    break;
                }
                
                case 0xE: // 8XYE: Shift VX left by 1, VF = MSB before shift
                    if (!config.shiftQuirks) {
                        // Original behavior: VX = VY then shift
                        V[x] = V[y];
                    }
                    V[0xF] = (V[x] & 0x80) >> 7; // Set VF to most significant bit
                    V[x] <<= 1;
                    break;
                    
                default:
                    std::cerr << "Unknown opcode: " << std::hex << opcode << std::endl;
                    break;
            }
            break;
            
        case 0x9000: // 9XY0: Skip next instruction if VX doesn't equal VY
            if (n == 0 && V[x] != V[y]) {
                pc += 2;
            }
            break;
            
        case 0xA000: // ANNN: Set I to the address NNN
            I = nnn;
            break;
            
        case 0xB000: // BNNN: Jump to the address NNN plus V0
            pc = nnn + V[0];
            break;
            
        case 0xC000: // CXNN: Set VX to a random number masked by NN
            V[x] = randByte(rng) & nn;
            break;
            
        case 0xD000: // DXYN: Draw a sprite at position VX, VY with N bytes of sprite data starting at address I
        {
            uint8_t xPos = V[x] % Display::WIDTH;
            uint8_t yPos = V[y] % Display::HEIGHT;
            V[0xF] = 0;
            
            for (uint8_t row = 0; row < n; row++) {
                if (yPos + row >= Display::HEIGHT) break;
                
                uint8_t spriteByte = memory->read(I + row);
                
                for (uint8_t col = 0; col < 8; col++) {
                    if (xPos + col >= Display::WIDTH) break;
                    
                    // Check if the current pixel in the sprite is set
                    bool spritePixel = (spriteByte & (0x80 >> col)) != 0;
                    
                    if (spritePixel) {
                        // If this causes any pixel to be erased, set VF to 1
                        if (display->getPixel(xPos + col, yPos + row)) {
                            V[0xF] = 1;
                        }
                        
                        // XOR the pixel
                        bool current = display->getPixel(xPos + col, yPos + row);
                        display->setPixel(xPos + col, yPos + row, current ^ spritePixel);
                    }
                }
            }
            break;
        }
        
        case 0xE000:
            switch (nn) {
                case 0x9E: // EX9E: Skip next instruction if key with the value of VX is pressed
                    if (keyboard->isKeyPressed(V[x])) {
                        pc += 2;
                    }
                    break;
                    
                case 0xA1: // EXA1: Skip next instruction if key with the value of VX is not pressed
                    if (!keyboard->isKeyPressed(V[x])) {
                        pc += 2;
                    }
                    break;
                    
                default:
                    std::cerr << "Unknown opcode: " << std::hex << opcode << std::endl;
                    break;
            }
            break;
            
        case 0xF000:
            switch (nn) {
                case 0x07: // FX07: Set VX to the value of the delay timer
                    V[x] = delayTimer;
                    break;
                    
                case 0x0A: // FX0A: Wait for a key press, store the value of the key in VX
                {
                    bool keyPressed = false;
                    for (uint8_t i = 0; i < 16; i++) {
                        if (keyboard->isKeyPressed(i)) {
                            V[x] = i;
                            keyPressed = true;
                            break;
                        }
                    }
                    
                    // If no key is pressed, repeat this instruction
                    if (!keyPressed) {
                        pc -= 2;
                    }
                    break;
                }
                
                case 0x15: // FX15: Set the delay timer to VX
                    delayTimer = V[x];
                    break;
                    
                case 0x18: // FX18: Set the sound timer to VX
                    soundTimer = V[x];
                    if (soundTimer > 0 && beepCallback) {
                        beepCallback(true);
                    }
                    break;
                    
                case 0x1E: // FX1E: Add VX to I, VF = 1 on overflow (undocumented)
                {
                    I += V[x];
                    // Some implementations set VF on range overflow
                    if (config.modernBehavior && I > 0xFFF) {
                        V[0xF] = 1;
                    }
                    break;
                }
                
                case 0x29: // FX29: Set I to the location of the sprite for the character in VX
                    I = FONT_START_ADDRESS + (V[x] & 0xF) * 5;
                    break;
                    
                case 0x33: // FX33: Store the BCD representation of VX at I, I+1, and I+2
                {
                    uint8_t value = V[x];
                    memory->write(I, value / 100);          // Hundreds digit
                    memory->write(I + 1, (value / 10) % 10); // Tens digit
                    memory->write(I + 2, value % 10);        // Ones digit
                    break;
                }
                
                case 0x55: // FX55: Store registers V0 through VX in memory starting at location I
                {
                    for (uint8_t i = 0; i <= x; i++) {
                        memory->write(I + i, V[i]);
                    }
                    
                    // Original behavior increments I
                    if (!config.loadStoreQuirks) {
                        I += x + 1;
                    }
                    break;
                }
                
                case 0x65: // FX65: Read registers V0 through VX from memory starting at location I
                {
                    for (uint8_t i = 0; i <= x; i++) {
                        V[i] = memory->read(I + i);
                    }
                    
                    // Original behavior increments I
                    if (!config.loadStoreQuirks) {
                        I += x + 1;
                    }
                    break;
                }
                
                default:
                    std::cerr << "Unknown opcode: " << std::hex << opcode << std::endl;
                    break;
            }
            break;
            
        default:
            std::cerr << "Unknown opcode: " << std::hex << opcode << std::endl;
            break;
    }
}

} // namespace Chip8Emu
