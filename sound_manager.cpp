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

void AudioManager::Initialize(std::string pcm_device,snd_pcm_format_t format,unsigned int rate,int channels){
	unsigned int pcm, tmp, dir;
	

	//rate = 192000
	//channels = 1
	//seconds = depends on sound track
	
	//pcm_device "default"
	/* Open the PCM device in playback mode */
	if (pcm = snd_pcm_open(&pcm_handle, pcm_device.c_str(),
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
						format) < 0) 
		printf("ERROR: Can't set format. %s\n", snd_strerror(pcm));

	if (pcm = snd_pcm_hw_params_set_channels(pcm_handle, params, channels) < 0) 
		printf("ERROR: Can't set channels number. %s\n", snd_strerror(pcm));

	if (pcm = snd_pcm_hw_params_set_rate_near(pcm_handle, params, &rate, 0) < 0) 
		printf("ERROR: Can't set rate. %s\n", snd_strerror(pcm));

	/* Write parameters */
	if (pcm = snd_pcm_hw_params(pcm_handle, params) < 0)
		printf("ERROR: Can't set harware parameters. %s\n", snd_strerror(pcm));

	/* Resume information */
	printf("PCM name: '%s'\n", snd_pcm_name(pcm_handle));
	printf("PCM state: %s\n", snd_pcm_state_name(snd_pcm_state(pcm_handle)));


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

void AudioManager::PlaySound(std::string sound_name){
	char *buff;
	int buff_size, loops;
	unsigned int tmp;
	unsigned int pcm;
	snd_pcm_uframes_t frames;
	//TODO:fix seconds
	int seconds = 1;
	/* Allocate buffer to hold single period */
	std::string sound_file_path = GetSoundFileRoot() + sound_name;
	snd_pcm_hw_params_get_period_size(params, &frames, 0);
	snd_pcm_hw_params_get_period_time(params, &tmp, NULL);
	snd_pcm_hw_params_get_channels(params, &tmp);

	buff_size = frames * SOUND_CHANNEL * 2 /* 2 -> sample size */;
	//buff = (char *) malloc(buff_size);
	buff = new char(buff_size);


	int fd = open(sound_file_path.c_str(),O_RDONLY);
	if(fd < 0){
		printf("Audio File is not Exist\n");
		return;
	}
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
	close(fd);
	return;
}
void AudioManager::StopSound(){
	//TODO 기능 구현

}

void AudioManager::SetSpeakerVolume(float volume){

}

std::string AudioManager::GetSoundFileRoot(){
	//TODO: fix hardcoding path
	return "/mnt/d/Beyless/0.project/2.firefly_rk3399/src/beyless_vending_terminal/sound/";
}