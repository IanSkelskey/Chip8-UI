#ifndef CHIP8AUDIO_H
#define CHIP8AUDIO_H

#include <QObject>
#include <QAudioOutput>
#include <QMediaPlayer>
#include "core/include/api/IAudio.hpp"

class Chip8Audio : public QObject, public Chip8Emu::API::IAudio
{
    Q_OBJECT

public:
    explicit Chip8Audio(QObject *parent = nullptr);
    ~Chip8Audio() override;

    // IAudio interface implementation
    void onSoundStateChange(bool active) override;

    // Set sound properties
    void setVolume(int volume); // 0-100

private:
    QMediaPlayer *player;
    QAudioOutput *audioOutput;
    QString beepSoundFile;
    bool isSoundPlaying;
};

#endif // CHIP8AUDIO_H
