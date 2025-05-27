#pragma once

#include <array>
#include <cstdint>
#include <random>
#include <string>
#include <memory>
#include <chrono>
#include <functional>

namespace Chip8Emu {

// Forward declarations
class Memory;
class Display;
class Keyboard;

class Chip8 {
public:
    Chip8();
    ~Chip8();

    // Initialize the emulator
    void initialize();
    
    // Load a ROM from file
    bool loadROM(const std::string& filename);
    
    // Load a ROM from memory
    bool loadROM(const uint8_t* data, size_t size);
    
    // Execute one CPU cycle
    void cycle();
    
    // Handle timers (should be called at 60Hz)
    void updateTimers();
    
    // Accessors
    const std::array<uint8_t, 16>& getRegisters() const { return V; }
    uint16_t getProgramCounter() const { return pc; }
    uint16_t getIndexRegister() const { return I; }
    uint8_t getDelayTimer() const { return delayTimer; }
    uint8_t getSoundTimer() const { return soundTimer; }
    
    // Callback for sound (client code should implement the actual sound)
    void setBeepCallback(std::function<void(bool)> callback) { beepCallback = callback; }
    
    // Access to components
    Display* getDisplay() const { return display.get(); }
    Keyboard* getKeyboard() const { return keyboard.get(); }
    
    // Reset the emulator
    void reset();
    
    // Configuration options
    struct Config {
        bool modernBehavior = true;  // Use modern CHIP-8 behavior (for ambiguous instructions)
        bool shiftQuirks = false;    // If true, 8XY6/8XYE only operate on VX, if false, they set VX to shifted VY
        bool loadStoreQuirks = false; // If true, FX55/FX65 don't increment I
        uint16_t cpuSpeed = 700;     // Instructions per second
    };
    
    void setConfig(const Config& config) { this->config = config; }
    const Config& getConfig() const { return config; }

private:
    // Memory (4KB)
    std::unique_ptr<Memory> memory;
    
    // Display (64x32 pixels)
    std::unique_ptr<Display> display;
    
    // Keyboard (16 keys)
    std::unique_ptr<Keyboard> keyboard;
    
    // Registers
    std::array<uint8_t, 16> V;  // V0-VF
    uint16_t I;                 // Index register
    uint16_t pc;                // Program counter
    uint8_t sp;                 // Stack pointer
    
    // Timers
    uint8_t delayTimer;
    uint8_t soundTimer;
    
    // Stack (16 levels)
    std::array<uint16_t, 16> stack;
    
    // RNG
    std::mt19937 rng;
    std::uniform_int_distribution<uint8_t> randByte;
    
    // Configuration
    Config config;
    
    // Sound callback
    std::function<void(bool)> beepCallback;
    
    // Opcode execution
    void executeOpcode(uint16_t opcode);
    
    // Load font into memory
    void loadFont();
};

}  // namespace Chip8Emu
