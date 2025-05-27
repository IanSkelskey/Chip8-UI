#ifndef CHIP8INPUT_H
#define CHIP8INPUT_H

#include <QObject>
#include <QMap>
#include <QSet>
#include "core/include/api/IInput.hpp"
#include <QMutex>
#include <QWaitCondition>

class Chip8Input : public QObject, public Chip8Emu::API::IInput
{
    Q_OBJECT

public:
    explicit Chip8Input(QObject *parent = nullptr);
    ~Chip8Input() override;

    // IInput interface implementation
    bool isKeyPressed(uint8_t key) const override;
    uint8_t waitForKeyPress() override;

    // Process keyboard events from Qt
    void handleKeyPress(int qtKey);
    void handleKeyRelease(int qtKey);

    // Set up default key mapping
    void setupDefaultKeyMapping();

    // Get/set key mapping
    QMap<int, uint8_t> getKeyMapping() const;
    void setKeyMapping(const QMap<int, uint8_t>& mapping);

private:
    QMap<int, uint8_t> keyMapping;    // Maps Qt::Key to Chip8 keys (0-F)
    QSet<uint8_t> pressedKeys;        // Currently pressed Chip8 keys
    mutable QMutex keyMutex;          // Mutex for thread-safe access
    
    // For the waitForKeyPress functionality
    bool waitingForKey;
    uint8_t lastKeyPressed;
    QMutex waitMutex;
    QWaitCondition keyWaitCondition;
};

#endif // CHIP8INPUT_H
