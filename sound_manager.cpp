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

void AudioManager::SetSpeakerVolume(float volume){

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
	if (pcm = snd_pcm_open(&pcm_handle, PCM_DEVICE.c_str(),
					SND_PCM_STREAM_PLAYBACK, 0) < 0) 
		printf("ERROR: Can't open \"%s\" PCM device. %s\n",
					PCM_DEVICE, snd_strerror(pcm));

	/* Allocate parameters object and fill it with default values*/
	snd_pcm_hw_params_alloca(&params);

	snd_pcm_hw_params_any(pcm_handle, params);
	//format = SND_PCM_FORMAT_S16_LE;
	/* Set parameters */
	if (pcm = snd_pcm_hw_params_set_access(pcm_handle, params,
					SND_PCM_ACCESS_RW_INTERLEAVED) < 0) 
		printf("ERROR: Can't set interleaved mode. %s\n", snd_strerror(pcm));

	if (pcm = snd_pcm_hw_params_set_format(pcm_handle, params,
						sound_file_info.format) < 0) 
		printf("ERROR: Can't set format. %s\n", snd_strerror(pcm));

	if (pcm = snd_pcm_hw_params_set_channels(pcm_handle, params, sound_file_info.channels) < 0) 
		printf("ERROR: Can't set channels number. %s\n", snd_strerror(pcm));

	if (pcm = snd_pcm_hw_params_set_rate_near(pcm_handle, params, &sound_file_info.bit_rate, 0) < 0) 
		printf("ERROR: Can't set rate. %s\n", snd_strerror(pcm));

	/* Write parameters */
	if (pcm = snd_pcm_hw_params(pcm_handle, params) < 0)
		printf("ERROR: Can't set harware parameters. %s\n", snd_strerror(pcm));

	/* Resume information */
	printf("PCM name: '%s'\n", snd_pcm_name(pcm_handle));
	printf("PCM state: %s\n", snd_pcm_state_name(snd_pcm_state(pcm_handle)));

	snd_pcm_hw_params_get_channels(params, &tmp);
	printf("channels: %i ", tmp);

	if (tmp == 1)
		printf("(mono)\n");
	else if (tmp == 2)
		printf("(stereo)\n");

	snd_pcm_hw_params_get_rate(params, &tmp, 0);
	printf("rate: %d bps\n", tmp);

	printf("seconds: %f\n", sound_file_info.play_seconds);	

	/* Allocate buffer to hold single period */
	snd_pcm_hw_params_get_period_size(params, &frames, 0);

	buff_size = frames * sound_file_info.channels * 2 /* 2 -> sample size */;
	buff = (char *) malloc(buff_size);

	snd_pcm_hw_params_get_period_time(params, &tmp, NULL);
	//std::string sound_file_path = GetSoundFileRoot() + sound_name;
	int fd = open(sound_file_info.file_path.c_str(),O_RDONLY);
	if(fd < 0 ){
		printf("file is not exist\n");
		goto FUNC_END;
		return;
	}
	for (loops = (sound_file_info.play_seconds * 1000000) / tmp; loops > 0; loops--) {
		
		if (pcm = read(fd, buff, buff_size) == 0) {
			printf("Early end of file.\n");
			goto FUNC_END;
			//return;
		}
		if (pcm = snd_pcm_writei(pcm_handle, buff, frames) == -EPIPE) {
			printf("XRUN.\n");
			snd_pcm_prepare(pcm_handle);
		} else if (pcm < 0) {
			printf("ERROR. Can't write to PCM device. %s\n", snd_strerror(pcm));
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
