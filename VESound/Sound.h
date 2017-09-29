#pragma once
#include <SFML/Audio.hpp>
class Sound
{
public:
    Sound(std::string path);
    ~Sound();

    void play();
    void pause();
    void stop();

    void setPitch(float val);
    float getPitch();

    void setVolume(float val);
    float getVolume();

    void setOffset(float val);
    float getOffset();

    void setLoop(bool val);
    bool getLoop();

    sf::SoundSource::Status getStatus();

protected:
    sf::SoundBuffer buffer;
    sf::Sound sound;
    bool ready = false;
};

