 #pragma once

#ifndef H_AUDIOMANAGER
#define H_AUDIOMANAGER
#include <alsa/asoundlib.h>
#include <stdio.h>
#include <iostream>

//#define PCM_DEVICE "default"

enum SOUND_TYPE{
    SOUND_TYPE_OPEN,
    SOUND_TYPE_CLOSE,
    SOUND_TYPE_GREETING
};

struct AudioFileInfo{
    std::string file_path;
    float play_seconds;
    snd_pcm_format_t format;
    unsigned int bit_rate;
    int channels;
};

class AudioManager
{
public:
    // Destructor
    ~AudioManager(void);

    void Initialize(std::string sound_name,float seconds,snd_pcm_format_t format,unsigned int rate,int channels);
    void Shutdown(void);    // Shutdown sound components

    // Singleton instance manip methods
    static AudioManager* GetInstance(void);
    
    void StopSound();
    void SetSpeakerVolume(long volume_percent);
    static void DestroyInstance(void);
    bool PlaySound(SOUND_TYPE sound_type);
    
private:

    static AudioManager* instance;  // Singleton instance
    AudioManager(void);  // Constructor

    const int BIT_RATE = 192000; // this can be changed if other sound file is used
    const int SOUND_CHANNEL = 1; // this can be changed if other sound file is used
    const float OPEN_VOICE_SEC = 1.2; // this can be changed if other sound file is used
    const float CLOSE_VOICE_SEC= 1.2; // this can be changed if other sound file is used
    const float GREETING_VOICE_SEC = 4.9; // this can be changed if other sound file is used
    const snd_pcm_format_t SOUND_FORMAT = SND_PCM_FORMAT_S16_LE;
    const std::string PCM_DEVICE = "default";

    std::string GetSoundFileRoot();
    void PlayOpenSound();
    void PlayCloseSound();
    void PlayGreetingSound();
    void Play(AudioFileInfo sound_file_info);
    snd_pcm_t *pcm_handle;

};

#endif