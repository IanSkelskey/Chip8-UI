#pragma once

#include <cstdint>

namespace Chip8Emu {
namespace API {

class IInput {
public:
    virtual ~IInput() = default;
    
    // Query if a key is currently pressed (0-F are valid key values)
    virtual bool isKeyPressed(uint8_t key) const = 0;
    
    // Optional: Wait for a keypress and return the key value
    virtual uint8_t waitForKeyPress() = 0;
};

} // namespace API
} // namespace Chip8Emu
