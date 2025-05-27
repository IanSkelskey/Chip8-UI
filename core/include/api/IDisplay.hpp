#pragma once

#include <array>
#include <cstdint>
#include <functional>

namespace Chip8Emu {
namespace API {

class IDisplay {
public:
    static constexpr int WIDTH = 64;
    static constexpr int HEIGHT = 32;
    
    virtual ~IDisplay() = default;
    
    // Called when the frame buffer is updated
    virtual void onFrameUpdate(const std::array<bool, WIDTH * HEIGHT>& frameBuffer) = 0;
};

} // namespace API
} // namespace Chip8Emu
