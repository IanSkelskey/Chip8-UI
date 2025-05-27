#include "chip8input.h"
#include <QKeyEvent>
#include <QMutexLocker>

Chip8Input::Chip8Input(QObject *parent)
    : QObject(parent)
    , waitingForKey(false)
    , lastKeyPressed(0)
{
    setupDefaultKeyMapping();
}

Chip8Input::~Chip8Input() = default;

bool Chip8Input::isKeyPressed(uint8_t key) const
{
    QMutexLocker locker(&keyMutex);
    return pressedKeys.contains(key);
}

uint8_t Chip8Input::waitForKeyPress()
{
    QMutexLocker locker(&waitMutex);
    
    // Set waiting flag
    waitingForKey = true;
    
    // Wait for a key press
    keyWaitCondition.wait(&waitMutex);
    
    // Reset waiting flag
    waitingForKey = false;
    
    return lastKeyPressed;
}

void Chip8Input::handleKeyPress(int qtKey)
{
    QMutexLocker locker(&keyMutex);
    
    // Check if this Qt key is mapped to a Chip8 key
    if (keyMapping.contains(qtKey)) {
        uint8_t chip8Key = keyMapping[qtKey];
        pressedKeys.insert(chip8Key);
        
        // Signal for waitForKeyPress if it's waiting
        QMutexLocker waitLocker(&waitMutex);
        if (waitingForKey) {
            lastKeyPressed = chip8Key;
            keyWaitCondition.wakeOne();
        }
    }
}

void Chip8Input::handleKeyRelease(int qtKey)
{
    QMutexLocker locker(&keyMutex);
    
    // Check if this Qt key is mapped to a Chip8 key
    if (keyMapping.contains(qtKey)) {
        uint8_t chip8Key = keyMapping[qtKey];
        pressedKeys.remove(chip8Key);
    }
}

void Chip8Input::setupDefaultKeyMapping()
{
    // Default key mapping for Chip8 keypad:
    // 1 2 3 4    ->  1 2 3 C
    // Q W E R    ->  4 5 6 D
    // A S D F    ->  7 8 9 E
    // Z X C V    ->  A 0 B F
    
    keyMapping.clear();
    
    keyMapping[Qt::Key_1] = 0x1;
    keyMapping[Qt::Key_2] = 0x2;
    keyMapping[Qt::Key_3] = 0x3;
    keyMapping[Qt::Key_4] = 0xC;
    
    keyMapping[Qt::Key_Q] = 0x4;
    keyMapping[Qt::Key_W] = 0x5;
    keyMapping[Qt::Key_E] = 0x6;
    keyMapping[Qt::Key_R] = 0xD;
    
    keyMapping[Qt::Key_A] = 0x7;
    keyMapping[Qt::Key_S] = 0x8;
    keyMapping[Qt::Key_D] = 0x9;
    keyMapping[Qt::Key_F] = 0xE;
    
    keyMapping[Qt::Key_Z] = 0xA;
    keyMapping[Qt::Key_X] = 0x0;
    keyMapping[Qt::Key_C] = 0xB;
    keyMapping[Qt::Key_V] = 0xF;
}

QMap<int, uint8_t> Chip8Input::getKeyMapping() const
{
    QMutexLocker locker(&keyMutex);
    return keyMapping;
}

void Chip8Input::setKeyMapping(const QMap<int, uint8_t>& mapping)
{
    QMutexLocker locker(&keyMutex);
    keyMapping = mapping;
}
