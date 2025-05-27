#include "Display.hpp"
#include <algorithm>

namespace Chip8Emu {

Display::Display() {
    clear();
}

void Display::clear() {
    frameBuffer.fill(false);
}

bool Display::getPixel(uint8_t x, uint8_t y) const {
    x = x % WIDTH;
    y = y % HEIGHT;
    return frameBuffer[y * WIDTH + x];
}

void Display::setPixel(uint8_t x, uint8_t y, bool state) {
    x = x % WIDTH;
    y = y % HEIGHT;
    frameBuffer[y * WIDTH + x] = state;
}

bool Display::drawSprite(uint8_t x, uint8_t y, const uint8_t* spriteData, uint8_t height) {
    bool collision = false;
    
    for (uint8_t row = 0; row < height; row++) {
        if (y + row >= HEIGHT) break;  // Don't draw outside screen bounds
        
        uint8_t spriteByte = spriteData[row];
        
        for (uint8_t col = 0; col < 8; col++) {
            if (x + col >= WIDTH) break;  // Don't draw outside screen bounds
            
            // Check if the current pixel in the sprite is set
            bool spritePixel = (spriteByte & (0x80 >> col)) != 0;
            
            if (spritePixel) {
                // XOR with the current pixel
                uint16_t index = (y + row) * WIDTH + (x + col);
                bool currentPixel = frameBuffer[index];
                
                // If this causes any pixel to be erased, set collision flag
                if (currentPixel) {
                    collision = true;
                }
                
                // XOR the pixel
                frameBuffer[index] = currentPixel ^ spritePixel;
            }
        }
    }
    
    return collision;
}

} // namespace Chip8Emu
