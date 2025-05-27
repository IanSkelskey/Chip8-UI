#pragma once

namespace Chip8Emu {
namespace API {

class IAudio {
public:
    virtual ~IAudio() = default;
    
    // Called when the sound state changes
    virtual void onSoundStateChange(bool active) = 0;
};

} // namespace API
} // namespace Chip8Emu
