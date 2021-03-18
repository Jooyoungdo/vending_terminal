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

void AudioManager::Initialize(unsigned int rate,int channels){
	unsigned int pcm, tmp, dir;
	

	//rate = 192000
	//channels = 1
	//seconds = depends on sound track
	unsigned int bit_rate = rate;

	/* Open the PCM device in playback mode */
	if (pcm = snd_pcm_open(&pcm_handle, PCM_DEVICE,
					SND_PCM_STREAM_PLAYBACK, 0) < 0) 
		printf("ERROR: Can't open \"%s\" PCM device. %s\n",
					PCM_DEVICE, snd_strerror(pcm));

	/* Allocate parameters object and fill it with default values*/
	snd_pcm_hw_params_alloca(&params);

	snd_pcm_hw_params_any(pcm_handle, params);

	/* Set parameters */
	if (pcm = snd_pcm_hw_params_set_access(pcm_handle, params,
					SND_PCM_ACCESS_RW_INTERLEAVED) < 0) 
		printf("ERROR: Can't set interleaved mode. %s\n", snd_strerror(pcm));

	if (pcm = snd_pcm_hw_params_set_format(pcm_handle, params,
						SND_PCM_FORMAT_S16_LE) < 0) 
		printf("ERROR: Can't set format. %s\n", snd_strerror(pcm));

	if (pcm = snd_pcm_hw_params_set_channels(pcm_handle, params, channels) < 0) 
		printf("ERROR: Can't set channels number. %s\n", snd_strerror(pcm));

	if (pcm = snd_pcm_hw_params_set_rate_near(pcm_handle, params, &bit_rate, 0) < 0) 
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

	return;
}

// Shutdown sound components
void AudioManager::Shutdown(void){
	
	if(pcm_handle !=nullptr){
		snd_pcm_drain(pcm_handle);
		snd_pcm_close(pcm_handle);
		pcm_handle = nullptr;
	}
}

#define TEST_SOUND
void AudioManager::PlaySound(std::string sound_name,int channels,int seconds){
	char *buff;
	int buff_size, loops;
	unsigned int tmp;
	unsigned int pcm;
	snd_pcm_uframes_t frames;
	/* Allocate buffer to hold single period */

#ifndef TEST_SOUND
	// snd_pcm_hw_params_get_period_size(params, &frames, 0);
	// snd_pcm_hw_params_get_period_time(params, &tmp, NULL);
	// snd_pcm_hw_params_get_channels(params, &tmp);
#else
	frames=1000;
	tmp = 1;
#endif	
	buff_size = frames * channels * 2 /* 2 -> sample size */;
	//buff = (char *) malloc(buff_size);
	buff = new char(buff_size);


	int fd = open(sound_name.c_str(),O_RDONLY);
	for (loops = (seconds * 1000000) / tmp; loops > 0; loops--){
		if (pcm = read(fd, buff, buff_size) == 0){
			printf("Early end of file.\n");
			if(buff !=nullptr){
				delete buff;
			}
			return;
		}
		if (pcm = snd_pcm_writei(pcm_handle, buff, frames) == -EPIPE){
			printf("XRUN.\n");
			snd_pcm_prepare(pcm_handle);
		}
		else if (pcm < 0){
			printf("ERROR. Can't write to PCM device. %s\n", snd_strerror(pcm));
		}
	}
	if (buff != nullptr)
	{
		delete buff;
	}
}
void AudioManager::StopSound(){
	//TODO 기능 구현

}

void AudioManager::SetSpeakerVolume(float volume){

}