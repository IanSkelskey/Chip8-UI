#pragma once

#include "IDisplay.hpp"
#include "IAudio.hpp"
#include "IInput.hpp"
#include "../Chip8.hpp"

#include <memory>
#include <string>
#include <functional>

namespace Chip8Emu {
namespace API {

class EmulatorAPI {
public:
    EmulatorAPI();
    ~EmulatorAPI();
    
    // Register interfaces
    void setDisplay(std::shared_ptr<IDisplay> display);
    void setAudio(std::shared_ptr<IAudio> audio);
    void setInput(std::shared_ptr<IInput> input);
    
    // ROM loading
    bool loadROM(const std::string& filename);
    bool loadROM(const uint8_t* data, size_t size);
    
    // Emulation control
    void reset();
    void runCycle();    // Execute a single CPU cycle
    void updateTimers(); // Should be called at 60Hz independently of CPU cycles
    
    // Execution control
    void pause();
    void resume();
    bool isPaused() const { return paused; }
    
    // Configuration
    void setSpeed(uint16_t instructionsPerSecond);
    void setConfig(const Chip8::Config& config);
    const Chip8::Config& getConfig() const;
    
private:
    std::unique_ptr<Chip8> emulator;
    std::shared_ptr<IDisplay> displayInterface;
    std::shared_ptr<IAudio> audioInterface;
    std::shared_ptr<IInput> inputInterface;
    
    bool paused;
    bool soundActive;
    
    // Setup input system that communicates with the emulator
    void setupInputSystem();
    
    // Audio callback function for the emulator
    void handleBeep(bool active);
};

} // namespace API
} // namespace Chip8Emu
