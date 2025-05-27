#include "../../include/api/EmulatorAPI.hpp"
#include "../../include/Display.hpp"

namespace Chip8Emu {
namespace API {

EmulatorAPI::EmulatorAPI() 
    : emulator(std::make_unique<Chip8>())
    , paused(true)
    , soundActive(false) {
    
    // Setup the beep callback
    emulator->setBeepCallback([this](bool active) {
        this->handleBeep(active);
    });
    
    // Initialize the emulator
    emulator->initialize();
}

EmulatorAPI::~EmulatorAPI() = default;

void EmulatorAPI::setDisplay(std::shared_ptr<IDisplay> display) {
    displayInterface = display;
}

void EmulatorAPI::setAudio(std::shared_ptr<IAudio> audio) {
    audioInterface = audio;
    
    // Update audio with current state
    if (audioInterface) {
        audioInterface->onSoundStateChange(soundActive);
    }
}

void EmulatorAPI::setInput(std::shared_ptr<IInput> input) {
    inputInterface = input;
    setupInputSystem();
}

bool EmulatorAPI::loadROM(const std::string& filename) {
    return emulator->loadROM(filename);
}

bool EmulatorAPI::loadROM(const uint8_t* data, size_t size) {
    return emulator->loadROM(data, size);
}

void EmulatorAPI::reset() {
    emulator->reset();
    soundActive = false;
    
    if (audioInterface) {
        audioInterface->onSoundStateChange(false);
    }
}

void EmulatorAPI::runCycle() {
    if (!paused && emulator) {
        // Run one emulation cycle
        emulator->cycle();
        
        // Update display if we have a display interface
        if (displayInterface) {
            displayInterface->onFrameUpdate(emulator->getDisplay()->getFrameBuffer());
        }
    }
}

void EmulatorAPI::updateTimers() {
    if (!paused && emulator) {
        emulator->updateTimers();
    }
}

void EmulatorAPI::pause() {
    paused = true;
    
    // Ensure sound is turned off when paused
    if (soundActive && audioInterface) {
        soundActive = false;
        audioInterface->onSoundStateChange(false);
    }
}

void EmulatorAPI::resume() {
    paused = false;
}

void EmulatorAPI::setSpeed(uint16_t instructionsPerSecond) {
    Chip8::Config config = emulator->getConfig();
    config.cpuSpeed = instructionsPerSecond;
    emulator->setConfig(config);
}

void EmulatorAPI::setConfig(const Chip8::Config& config) {
    emulator->setConfig(config);
}

const Chip8::Config& EmulatorAPI::getConfig() const {
    return emulator->getConfig();
}

void EmulatorAPI::setupInputSystem() {
    // Connect input system to emulator's keyboard
    // This will be implemented in a real implementation that 
    // adapts IInput to the actual keyboard implementation
}

void EmulatorAPI::handleBeep(bool active) {
    soundActive = active;
    
    if (audioInterface) {
        audioInterface->onSoundStateChange(active);
    }
}

} // namespace API
} // namespace Chip8Emu
