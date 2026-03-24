#include <iostream>

#include "sound_engine.h"


SoundEngine::SoundEngine() {}

SoundEngine::~SoundEngine() {
    for (auto* sound : SoundList) {
        ma_sound_uninit(sound);
        delete sound;
    }
    ma_engine_uninit(&Engine);
}

void SoundEngine::Init(float volume)
{
    Result = ma_engine_init(NULL, &Engine);
    if (Result != MA_SUCCESS) {
        std::cout << "Error initializing sound engine." << std::endl;
    }
    ma_engine_set_volume(&Engine, volume);
}

void SoundEngine::LoadSound(const char *filePath, bool isMusic)
{
    ma_sound* newSound = new ma_sound();

    Result = ma_sound_init_from_file(&Engine, filePath, MA_SOUND_FLAG_DECODE, NULL, NULL, newSound);
    if (Result != MA_SUCCESS) {
        std::cout << "Error loading audio: " << filePath << std::endl;
        delete newSound;
    }
    else
    {
        if (isMusic) {
            ma_sound_set_looping(newSound, MA_TRUE);
        }
        SoundList.push_back(newSound);
        std::cout << "Audio loaded: " << filePath << std::endl;
    }
}

void SoundEngine::StartSound(SoundEnum sound)
{
    int index = static_cast<int>(sound);
    
    if (index < SoundList.size()) {
        Result = ma_sound_start(SoundList[index]);
        if (Result != MA_SUCCESS) {
            std::cout << "Error starting audio." << std::endl;
        }
    }
}

void SoundEngine::StopSound(SoundEnum sound)
{
    int index = static_cast<int>(sound);
    
    if (index < SoundList.size()) {

        Result = ma_sound_stop(SoundList[index]);
        if (Result != MA_SUCCESS) {
            std::cout << "Error stopping audio." << std::endl;
        }
    }
}

