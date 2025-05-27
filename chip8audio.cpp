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
    
    // Set volume to a higher level to make sure it's audible
    audioOutput->setVolume(0.8); // 80% volume
    
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
    
    // Connect media status changes for debugging
    connect(player, &QMediaPlayer::mediaStatusChanged, this, [this](QMediaPlayer::MediaStatus status) {
        qDebug() << "Media status changed: " << status;
    });
    
    // Connect playback state changes for debugging
    connect(player, &QMediaPlayer::playbackStateChanged, this, [this](QMediaPlayer::PlaybackState state) {
        qDebug() << "Playback state changed: " << state;
    });
    
    qDebug() << "Chip8Audio initialized";
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
        qDebug() << "Started playing sound - Player state: " << player->playbackState();
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
    float volumeLevel = volume / 100.0f;
    audioOutput->setVolume(volumeLevel);
    qDebug() << "Audio volume set to" << volume << "% (" << volumeLevel << ")";
}
