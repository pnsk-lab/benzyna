#include <ba_runtime.h>

static void data_callback(ma_device* device, void* output, const void* input, ma_uint32 frame) {
	ba_audio_t* audio = device->pUserData;

	ma_mutex_lock(&audio->mutex);
	ma_mutex_unlock(&audio->mutex);
}

ba_audio_t* ba_audio_open(void) {
	ba_audio_t* audio = malloc(sizeof(*audio));

	memset(audio, 0, sizeof(*audio));

	audio->config			= ma_device_config_init(ma_device_type_playback);
	audio->config.playback.format	= ma_format_s16;
	audio->config.playback.channels = 2;
	audio->config.sampleRate	= 48000;
	audio->config.dataCallback	= data_callback;
	audio->config.pUserData		= audio;

	ma_mutex_init(&audio->mutex);

	if(ma_device_init(NULL, &audio->config, &audio->device) != MA_SUCCESS) {
		ba_audio_close(audio);
		return NULL;
	}

	audio->init = ba_true;
	if(ma_device_start(&audio->device) != MA_SUCCESS) {
		ba_audio_close(audio);
		return NULL;
	}

	return audio;
}

void ba_audio_close(ba_audio_t* audio) {
	if(audio->init) {
		ma_device_uninit(&audio->device);
	}

	ma_mutex_uninit(&audio->mutex);

	free(audio);
}
