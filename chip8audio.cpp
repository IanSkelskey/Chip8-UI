#include "chip8audio.h"
#include <QUrl>
#include <QDir>
#include <QStandardPaths>

Chip8Audio::Chip8Audio(QObject *parent)
    : QObject(parent)
    , isSoundPlaying(false)
{
    // Create the media player and audio output
    player = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);
    player->setAudioOutput(audioOutput);
    
    // Set default volume
    audioOutput->setVolume(0.5); // 50%
    
    // Default beep sound file path - this needs to be created or included in your resources
    beepSoundFile = ":/sounds/beep.wav";
    
    // Configure player for looping
    player->setSource(QUrl(beepSoundFile));
    player->setLoops(QMediaPlayer::Infinite);
}

Chip8Audio::~Chip8Audio()
{
    // Ensure sound is stopped
    if (isSoundPlaying) {
        player->stop();
    }
}

void Chip8Audio::onSoundStateChange(bool active)
{
    if (active && !isSoundPlaying) {
        // Start playing the beep sound
        player->play();
        isSoundPlaying = true;
    } else if (!active && isSoundPlaying) {
        // Stop playing the beep sound
        player->stop();
        isSoundPlaying = false;
    }
}

void Chip8Audio::setVolume(int volume)
{
    // Convert from 0-100 scale to 0.0-1.0 scale
    audioOutput->setVolume(volume / 100.0);
}
