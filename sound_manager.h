 #pragma once

#ifndef H_AUDIOMANAGER
#define H_AUDIOMANAGER
#include <alsa/asoundlib.h>
#include <stdio.h>
#include <iostream>
#include "logger.h"
//#define PCM_DEVICE "default"

enum SOUND_TYPE{
    SOUND_TYPE_OPEN,
    SOUND_TYPE_CLOSE,
    SOUND_TYPE_GREETING,
    SOUND_TYPE_REGREETING,
    SOUND_TYPE_GOODBYE
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
    std::string GetSoundFileRoot();
    int GetSoundCardCount();
private:

    static AudioManager* instance;  // Singleton instance
    AudioManager(void);  // Constructor

    const unsigned int BIT_RATE = 192000; // this can be changed if other sound file is used
    const int SOUND_CHANNEL = 1; // this can be changed if other sound file is used
    const float OPEN_VOICE_SEC = 1.2; // this can be changed if other sound file is used
    const float CLOSE_VOICE_SEC= 1.2; // this can be changed if other sound file is used
    const float GREETING_VOICE_SEC = 4.3; // this can be changed if other sound file is used
    const float REGREETING_VOICE_SEC = 1.7; // this can be changed if other sound file is used
    const float GOODBYE_VOICE_SEC = 1.2; // this can be changed if other sound file is used
    const snd_pcm_format_t SOUND_FORMAT = SND_PCM_FORMAT_S16_LE;
    const std::string PCM_DEVICE = "default";

    /**
     * @brief : Play Open Sound 
     * 
     */
    void PlayOpenSound();

    /**
     * @brief : Play Close Sound 
     * 
     */
    void PlayCloseSound();

    /**
     * @brief : Play Greeting Sound 
     * 
     */
    void PlayGreetingSound();

    /**
     * @brief : Play Greeting Sound to the customers, not the first time
     * 
     */
    void PlayReGreetingSound();

    /**
     * @brief : Play Goodbye Sound
     * 
     */
    void PlayGoodByeSound();

    /**
     * @brief : Play sound with sound file information
     * 
     * @param sound_file_info : include sound file information
     */
    void Play(AudioFileInfo sound_file_info);

    snd_pcm_t *pcm_handle; // pulse control module handle

    logger log = logger("SOUND"); // logger object

};

#endif