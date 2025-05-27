#pragma once

#include <array>
#include <cstdint>

namespace Chip8Emu {

class Memory {
public:
    Memory();
    ~Memory() = default;
    
    // Read a byte from memory
    uint8_t read(uint16_t address) const;
    
    // Write a byte to memory
    void write(uint16_t address, uint8_t value);
    
    // Load data into memory at specified address
    void load(uint16_t address, const uint8_t* data, size_t size);
    
    // Clear memory
    void clear();
    
    // Get raw access to memory (for debugging)
    const std::array<uint8_t, 4096>& getRawMemory() const { return memory; }

private:
    std::array<uint8_t, 4096> memory;
};

} // namespace Chip8Emu
