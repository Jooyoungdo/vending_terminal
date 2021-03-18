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

    void Initialize(unsigned int rate,int channels);
    void Shutdown(void);    // Shutdown sound components

    // Singleton instance manip methods
    static AudioManager* GetInstance(void);
    void PlaySound(std::string sound_name,int channels,int seconds);
    void StopSound();
    void SetSpeakerVolume(float volume);
    static void DestroyInstance(void);
private:
    static AudioManager* instance;  // Singleton instance
    AudioManager(void);  // Constructor
    snd_pcm_t *pcm_handle;
    snd_pcm_hw_params_t *params;
};

#endif