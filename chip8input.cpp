#include "chip8input.h"
#include <QKeyEvent>
#include <QMutexLocker>
#include <QDebug>

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
    qDebug() << "Waiting for key press...";
    
    // Wait for a key press
    keyWaitCondition.wait(&waitMutex);
    
    // Reset waiting flag
    waitingForKey = false;
    
    qDebug() << "Key press detected in wait:" << lastKeyPressed;
    return lastKeyPressed;
}

void Chip8Input::handleKeyPress(int qtKey)
{
    QMutexLocker locker(&keyMutex);
    
    // Check if this Qt key is mapped to a Chip8 key
    if (keyMapping.contains(qtKey)) {
        uint8_t chip8Key = keyMapping[qtKey];
        
        // Add to pressed keys set
        pressedKeys.insert(chip8Key);
        
        // Update last key pressed
        lastKeyPressed = chip8Key;
        
        qDebug() << "Key pressed - Qt:" << qtKey << "Chip8:" << chip8Key;
        
        // If waiting for a key press, signal it
        if (waitingForKey) {
            QMutexLocker waitLocker(&waitMutex);
            keyWaitCondition.wakeAll();
        }
    } else {
        qDebug() << "Unmapped key pressed:" << qtKey;
    }
}

void Chip8Input::handleKeyRelease(int qtKey)
{
    QMutexLocker locker(&keyMutex);
    
    // Check if this Qt key is mapped to a Chip8 key
    if (keyMapping.contains(qtKey)) {
        uint8_t chip8Key = keyMapping[qtKey];
        
        // Remove from pressed keys set
        pressedKeys.remove(chip8Key);
        
        qDebug() << "Key released - Qt:" << qtKey << "Chip8:" << chip8Key;
    }
}

void Chip8Input::setupDefaultKeyMapping()
{
    // Standard CHIP-8 keypad layout:
    // 1 2 3 C    ->    1 2 3 4
    // 4 5 6 D    ->    Q W E R
    // 7 8 9 E    ->    A S D F
    // A 0 B F    ->    Z X C V
    
    keyMapping.clear();
    
    // Row 1
    keyMapping[Qt::Key_1] = 0x1;
    keyMapping[Qt::Key_2] = 0x2;
    keyMapping[Qt::Key_3] = 0x3;
    keyMapping[Qt::Key_4] = 0xC;
    
    // Row 2
    keyMapping[Qt::Key_Q] = 0x4;
    keyMapping[Qt::Key_W] = 0x5;
    keyMapping[Qt::Key_E] = 0x6;
    keyMapping[Qt::Key_R] = 0xD;
    
    // Row 3
    keyMapping[Qt::Key_A] = 0x7;
    keyMapping[Qt::Key_S] = 0x8;
    keyMapping[Qt::Key_D] = 0x9;
    keyMapping[Qt::Key_F] = 0xE;
    
    // Row 4
    keyMapping[Qt::Key_Z] = 0xA;
    keyMapping[Qt::Key_X] = 0x0;
    keyMapping[Qt::Key_C] = 0xB;
    keyMapping[Qt::Key_V] = 0xF;
    
    qDebug() << "Default key mapping set up with" << keyMapping.size() << "keys";
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
    qDebug() << "Custom key mapping set with" << keyMapping.size() << "keys";
}
