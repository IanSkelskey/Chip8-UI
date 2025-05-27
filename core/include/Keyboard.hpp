#pragma once

#include <array>
#include <cstdint>

namespace Chip8Emu {

class Keyboard {
public:
    Keyboard();
    ~Keyboard() = default;
    
    // Set key state (true = pressed, false = released)
    void setKeyState(uint8_t key, bool pressed);
    
    // Check if a key is pressed
    bool isKeyPressed(uint8_t key) const;
    
    // Wait for a key press and return the key (blocking)
    uint8_t waitForKeyPress();
    
    // Register a key press
    void keyPressed(uint8_t key);
    
    // Reset all keys to not pressed
    void reset();

private:
    std::array<bool, 16> keyState;
    uint8_t lastKeyPressed;
    bool keyPressWaiting;
};

} // namespace Chip8Emu
