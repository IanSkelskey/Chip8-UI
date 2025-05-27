#include "Keyboard.hpp"

namespace Chip8Emu {

Keyboard::Keyboard() : lastKeyPressed(0), keyPressWaiting(false) {
    reset();
}

void Keyboard::setKeyState(uint8_t key, bool pressed) {
    if (key < keyState.size()) {
        keyState[key] = pressed;
        if (pressed) {
            lastKeyPressed = key;
            keyPressWaiting = true;
        }
    }
}

bool Keyboard::isKeyPressed(uint8_t key) const {
    if (key < keyState.size()) {
        return keyState[key];
    }
    return false;
}

uint8_t Keyboard::waitForKeyPress() {
    keyPressWaiting = false;
    // This is a non-blocking implementation
    // The actual waiting happens in the Chip8 class in the FX0A instruction
    // We just return the last key pressed when a key is detected
    return lastKeyPressed;
}

void Keyboard::keyPressed(uint8_t key) {
    lastKeyPressed = key;
    keyPressWaiting = true;
}

void Keyboard::reset() {
    keyState.fill(false);
    keyPressWaiting = false;
}

} // namespace Chip8Emu
