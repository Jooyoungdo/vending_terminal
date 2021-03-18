 #pragma once

#ifndef H_AUDIOMANAGER
#define H_AUDIOMANAGER
#include <alsa/asoundlib.h>
#include <stdio.h>
#include <iostream>

#define PCM_DEVICE "default"

class AudioManager
{
public:
    // Destructor
    ~AudioManager(void);

    void Initialize(std::string pcm_device,snd_pcm_format_t format,unsigned int rate,int channels);
    void Shutdown(void);    // Shutdown sound components

    // Singleton instance manip methods
    static AudioManager* GetInstance(void);
    void PlaySound(std::string sound_name);
    void StopSound();
    void SetSpeakerVolume(float volume);
    static void DestroyInstance(void);

    const int SAMPLE_RATE = 192000; // this can be changed if other sound file is used
    const int SOUND_CHANNEL = 1; // this can be changed if other sound file is used
private:
    static AudioManager* instance;  // Singleton instance
    AudioManager(void);  // Constructor
    std::string GetSoundFileRoot();
    snd_pcm_t *pcm_handle;
    snd_pcm_hw_params_t *params;
};

#endif