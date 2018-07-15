#include "stdafx.h"
#include "Sound.h"
#include "Media.h"
 
Sound::Sound(std::string mediakey)
{
    if (buffer.loadFromFile(Media::getPath(mediakey))) {
        sound.setBuffer(buffer);
        ready = true;
    }
    else {
        printf("SOUND LOADING ERROR %s\n", mediakey.c_str());
    }
}


Sound::~Sound()
{
}

void Sound::play()
{
    if (ready && sound.getStatus() != sound.Playing) {
        sound.play();
    }
}

void Sound::pause()
{
    sound.pause();
}

void Sound::stop()
{
    sound.stop();
}

void Sound::setPitch(float val)
{
    sound.setPitch(val);
}

float Sound::getPitch()
{
    return sound.getPitch();
}

void Sound::setVolume(float val)
{
    sound.setVolume(val);
}

float Sound::getVolume()
{
    return sound.getVolume();
}

void Sound::setOffset(float val)
{
    sound.setPlayingOffset(sf::seconds(val));
}

float Sound::getOffset()
{
    return sound.getPlayingOffset().asSeconds();
}

void Sound::setLoop(bool val)
{
    sound.setLoop(val);
}

bool Sound::getLoop()
{
    return sound.getLoop();
}

sf::SoundSource::Status Sound::getStatus()
{
    return sound.getStatus();
}
