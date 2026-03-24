#ifndef SOUND_ENGINE_H
#define SOUND_ENGINE_H

#include "miniaudio.h"
#include <vector>

enum SoundEnum{
    MUSIC,
    SFX_JUMP,
    SFX_ACQUIRE,
    SFX_FAIL
};

class SoundEngine
{
public:
    ma_engine Engine;
    ma_result Result;
    std::vector<ma_sound*> SoundList;



    SoundEngine();
    ~SoundEngine();

    void Init(float volume);

    void LoadSound(const char *filePath, bool isMusic = false);
    void StartSound(SoundEnum sound);
    void StopSound(SoundEnum sound);

private:


};
#endif
