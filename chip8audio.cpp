#include "chip8audio.h"
#include <QUrl>
#include <QDir>
#include <QStandardPaths>
#include <QDebug>

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
    
    // Default beep sound file path
    beepSoundFile = ":/sounds/beep.wav";
    
    // Check if the sound file exists in resources
    if (QFile::exists(beepSoundFile)) {
        qDebug() << "Sound file found: " << beepSoundFile;
    } else {
        qDebug() << "Sound file NOT found: " << beepSoundFile;
        // Fall back to a generated tone if possible
        beepSoundFile = ":/sounds/fallback_beep.wav";
    }
    
    // Configure player for looping
    player->setSource(QUrl(beepSoundFile));
    player->setLoops(QMediaPlayer::Infinite);
    
    // Connect error signal to debug output
    connect(player, &QMediaPlayer::errorOccurred, this, [this](QMediaPlayer::Error error, const QString &errorString) {
        qDebug() << "Media player error: " << error << " - " << errorString;
    });
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
    qDebug() << "Sound state change requested: " << (active ? "ON" : "OFF");
    
    if (active && !isSoundPlaying) {
        // Start playing the beep sound
        player->play();
        isSoundPlaying = true;
        qDebug() << "Started playing sound";
    } else if (!active && isSoundPlaying) {
        // Stop playing the beep sound
        player->stop();
        isSoundPlaying = false;
        qDebug() << "Stopped playing sound";
    }
}

void Chip8Audio::setVolume(int volume)
{
    // Convert from 0-100 scale to 0.0-1.0 scale
    audioOutput->setVolume(volume / 100.0);
}
