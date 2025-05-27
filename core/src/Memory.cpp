#include "Memory.hpp"
#include <algorithm>
#include <stdexcept>

namespace Chip8Emu {

Memory::Memory() {
    clear();
}

uint8_t Memory::read(uint16_t address) const {
    if (address >= memory.size()) {
        throw std::out_of_range("Memory access out of bounds");
    }
    return memory[address];
}

void Memory::write(uint16_t address, uint8_t value) {
    if (address >= memory.size()) {
        throw std::out_of_range("Memory access out of bounds");
    }
    memory[address] = value;
}

void Memory::load(uint16_t address, const uint8_t* data, size_t size) {
    if (address + size > memory.size()) {
        throw std::out_of_range("Memory load out of bounds");
    }
    std::copy(data, data + size, memory.begin() + address);
}

void Memory::clear() {
    memory.fill(0);
}

} // namespace Chip8Emu
