#pragma once

#include <array>
#include <cstdint>

namespace Chip8Emu {

class Display {
public:
    static constexpr int WIDTH = 64;
    static constexpr int HEIGHT = 32;
    
    Display();
    ~Display() = default;
    
    // Clear the display
    void clear();
    
    // Draw a sprite at position (x,y) with height and data from memory
    // Returns true if any pixels were flipped from set to unset
    bool drawSprite(uint8_t x, uint8_t y, const uint8_t* spriteData, uint8_t height);
    
    // Get the state of a pixel
    bool getPixel(uint8_t x, uint8_t y) const;
    
    // Set a pixel directly (for testing)
    void setPixel(uint8_t x, uint8_t y, bool state);
    
    // Get raw access to display buffer (for rendering)
    const std::array<bool, WIDTH * HEIGHT>& getFrameBuffer() const { return frameBuffer; }

private:
    std::array<bool, WIDTH * HEIGHT> frameBuffer;
};

} // namespace Chip8Emu
