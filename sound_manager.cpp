#include "sound_manager.h"

AudioManager* AudioManager::instance = 0;

AudioManager::AudioManager(void){}

AudioManager::~AudioManager(void)
{
	Shutdown();
}

AudioManager* AudioManager::GetInstance(void)
{
	if(instance == 0)
		instance = new AudioManager();
	return instance;
}

void AudioManager::DestroyInstance(void){
	if(instance){
		delete instance;
		instance = NULL;
	}
}

// Shutdown sound components
void AudioManager::Shutdown(void){
	if(pcm_handle !=nullptr){
		snd_pcm_drain(pcm_handle);
		snd_pcm_close(pcm_handle);
		pcm_handle = nullptr;
	}
}

void AudioManager::StopSound(){
	//TODO 기능 구현

}

void AudioManager::SetSpeakerVolume(long volume_percent){
	long min, max;
    snd_mixer_t *handle=NULL;
    snd_mixer_selem_id_t *sid;
    const char *card = "default";
	int card_number = -1;
    //const char *selem_name = "Master";

    snd_mixer_open(&handle, 0);
	if(handle == NULL){
		log.print_log("open mixer fail");
		return;
	}

	if(GetSoundCardCount() <= 0){
		log.print_log("sound card is not exist");
		return;
	}
		
	
    snd_mixer_attach(handle, card);
    snd_mixer_selem_register(handle, NULL, NULL);
    snd_mixer_load(handle);

    snd_mixer_selem_id_alloca(&sid);
    snd_mixer_selem_id_set_index(sid, 0);
    snd_mixer_selem_id_set_name(sid, "Speaker");
    
    snd_mixer_elem_t* elem = snd_mixer_find_selem(handle, sid);

    snd_mixer_selem_get_playback_volume_range(elem, &min, &max);
    snd_mixer_selem_set_playback_volume_all(elem, volume_percent * max / 100);

    snd_mixer_close(handle);
	return;
}


std::string AudioManager::GetSoundFileRoot(){
	//TODO: fix hardcoding path
	return "/home/firefly/beyless_vending_terminal/sound/";
}

bool AudioManager::PlaySound(SOUND_TYPE sound_type){

	switch (sound_type)
	{
	case SOUND_TYPE_OPEN:
		PlayOpenSound();
		break;
	case SOUND_TYPE_CLOSE:
		PlayCloseSound();
		break;
	case SOUND_TYPE_GREETING:
		PlayGreetingSound();
		break;		
	default:
		return false;
		break;
	}
	return true;
}

void AudioManager::PlayOpenSound(){
	AudioFileInfo audio = {
		GetSoundFileRoot()+"open_voice.wav",
		AudioManager::OPEN_VOICE_SEC,
		AudioManager::SOUND_FORMAT,
		AudioManager::BIT_RATE,
		AudioManager::SOUND_CHANNEL};
	Play(audio);
	return;
}
void AudioManager::PlayCloseSound(){
	AudioFileInfo audio = {
		GetSoundFileRoot()+"close_voice.wav",
		AudioManager::CLOSE_VOICE_SEC,
		AudioManager::SOUND_FORMAT,
		AudioManager::BIT_RATE,
		AudioManager::SOUND_CHANNEL};
	Play(audio);
	return;
}
void AudioManager::PlayGreetingSound(){
	AudioFileInfo audio = {
		GetSoundFileRoot()+"greeting_voice.wav",
		AudioManager::GREETING_VOICE_SEC,
		AudioManager::SOUND_FORMAT,
		AudioManager::BIT_RATE,
		AudioManager::SOUND_CHANNEL};
	Play(audio);
	return;
}

void AudioManager::Play(AudioFileInfo sound_file_info){
	unsigned int pcm, tmp, dir;
	char *buff;
	int buff_size, loops;
	snd_pcm_uframes_t frames;
    snd_pcm_hw_params_t *params;

	/* Open the PCM device in playback mode */
	if (pcm = snd_pcm_open(&pcm_handle, PCM_DEVICE.c_str(),SND_PCM_STREAM_PLAYBACK, 0) < 0){
		log.print_log("ERROR: Can't open "+PCM_DEVICE+" PCM device. "+ std::string(snd_strerror(pcm)));						   
	}

	/* Allocate parameters object and fill it with default values*/
	snd_pcm_hw_params_alloca(&params);

	snd_pcm_hw_params_any(pcm_handle, params);
	//format = SND_PCM_FORMAT_S16_LE;
	/* Set parameters */
	if (pcm = snd_pcm_hw_params_set_access(pcm_handle, params,
					SND_PCM_ACCESS_RW_INTERLEAVED) < 0) 
		log.print_log("ERROR: Can't set interleaved mode \n"+ std::string(snd_strerror(pcm)));

	if (pcm = snd_pcm_hw_params_set_format(pcm_handle, params,
						sound_file_info.format) < 0) 
		log.print_log("ERROR: Can't set format. %s\n"+ std::string(snd_strerror(pcm)));

	if (pcm = snd_pcm_hw_params_set_channels(pcm_handle, params, sound_file_info.channels) < 0) 
		log.print_log("ERROR: Can't set channels number. %s\n"+ std::string(snd_strerror(pcm)));

	if (pcm = snd_pcm_hw_params_set_rate_near(pcm_handle, params, &sound_file_info.bit_rate, 0) < 0) 
		log.print_log("ERROR: Can't set rate. %s\n"+ std::string(snd_strerror(pcm)));

	/* Write parameters */
	if (pcm = snd_pcm_hw_params(pcm_handle, params) < 0)
		log.print_log("ERROR: Can't set harware parameters. %s\n"+ std::string(snd_strerror(pcm)));
	snd_pcm_hw_params_get_channels(params, &tmp);
	snd_pcm_hw_params_get_rate(params, &tmp, 0);
	/* Allocate buffer to hold single period */
	snd_pcm_hw_params_get_period_size(params, &frames, 0);

	buff_size = frames * sound_file_info.channels * 2 /* 2 -> sample size */;
	buff = (char *) malloc(buff_size);

	snd_pcm_hw_params_get_period_time(params, &tmp, NULL);
	//std::string sound_file_path = GetSoundFileRoot() + sound_name;
	int fd = open(sound_file_info.file_path.c_str(),O_RDONLY);
	if(fd < 0 ){
		log.print_log("Sound File is not exist\n");
		goto FUNC_END;
		return;
	}
	for (loops = (sound_file_info.play_seconds * 1000000) / tmp; loops > 0; loops--) {
		
		if (pcm = read(fd, buff, buff_size) == 0) {
			log.print_log("Check Buffer Size\n");
			goto FUNC_END;
			//return;
		}
		if (pcm = snd_pcm_writei(pcm_handle, buff, frames) == -EPIPE) {
			log.print_log("Playing Sound has problem\n");
			snd_pcm_prepare(pcm_handle);
		} else if (pcm < 0) {
			log.print_log("ERROR. Can't write to PCM device "+ std::string(snd_strerror(pcm)));
		}
	}
FUNC_END:
	if(buff != NULL){
		free(buff);
	}
	if(pcm_handle != NULL){
		snd_pcm_drain(pcm_handle);
		snd_pcm_close(pcm_handle);
	}
	return;
}

int AudioManager::GetSoundCardCount(){
	int total_cards = 0;   // No cards found yet
    int card_num = -1;     // Start with first card
    int err;
	int max_sound_cards = 10;
    for (int i =0 ; i< max_sound_cards;i++) {
        // Get next sound card's card number.
        if ((err = snd_card_next(&card_num)) < 0) {
			log.print_log("Can't get the next card number: "+ std::string( snd_strerror(err)));
            break;
        }
        if (card_num < 0)
            break;
        ++total_cards;   // Another card found, so bump the count
    }
    snd_config_update_free_global();
	return total_cards;
}
