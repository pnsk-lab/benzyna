#include <ba_runtime.h>

static void data_callback(ma_device* device, void* output, const void* input, ma_uint32 frame) {
	ba_audio_t* audio = device->pUserData;
	int	    i;
	float*	    buffer = malloc(sizeof(*buffer) * 2 * frame);

	ma_mutex_lock(&audio->mutex);
	for(i = 0; i < arrlen(audio->streams); i++) {
		short* sbuffer;
		int    n;

		if(audio->streams[i]->paused || audio->streams[i]->read == NULL) continue;

		n	= (double)frame * audio->streams[i]->rate / ba_audio_rate;
		sbuffer = malloc(sizeof(*sbuffer) * 2 * n);

		memset(sbuffer, 0, sizeof(*sbuffer) * 2 * n);
		audio->streams[i]->read(audio->streams[i], sbuffer, n);

		free(sbuffer);
	}
	ma_mutex_unlock(&audio->mutex);
}

ba_audio_t* ba_audio_open(void) {
	ba_audio_t* audio = malloc(sizeof(*audio));

	memset(audio, 0, sizeof(*audio));

	audio->config			= ma_device_config_init(ma_device_type_playback);
	audio->config.playback.format	= ma_format_s16;
	audio->config.playback.channels = 2;
	audio->config.sampleRate	= ba_audio_rate;
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

void ba_audio_lock(ba_audio_t* audio) {
	ma_mutex_lock(&audio->mutex);
}

void ba_audio_unlock(ba_audio_t* audio) {
	ma_mutex_unlock(&audio->mutex);
}

ba_audio_stream_t* ba_audio_stream_new(ba_audio_t* audio) {
	ba_audio_stream_t* stream = malloc(sizeof(*stream));

	memset(stream, 0, sizeof(*stream));

	stream->audio  = audio;
	stream->paused = ba_true;

	ba_audio_lock(audio);
	arrput(audio->streams, stream);
	ba_audio_unlock(audio);

	return stream;
}

void ba_audio_stream_init(ba_audio_stream_t* stream) {
	ba_audio_lock(stream->audio);

	ba_audio_unlock(stream->audio);
}

void ba_audio_stream_free(ba_audio_stream_t* stream) {
	int i;

	ba_audio_lock(stream->audio);
	for(i = 0; i < arrlen(stream->audio->streams); i++) {
		if(stream->audio->streams[i] == stream) {
			arrdel(stream->audio->streams, i);
		}
	}
	ba_audio_unlock(stream->audio);

	if(stream->processor != NULL) {
		free(stream->processor);
	}

	free(stream);
}

void ba_audio_stream_init(ba_audio_stream_t* stream) {
	stream->processor = malloc(sizeof(*stream->processor));

	memset(stream->processor, 0, sizeof(*stream->processor));
}

void ba_audio_stream_set_paused(ba_audio_stream* stream, ba_bool paused) {
	ba_audio_lock(stream->audio);
	stream->paused = paused;
	ba_audio_unlock(stream->audio);
}

ba_bool ba_audio_stream_get_paused(ba_audio_stream* stream, ba_bool paused) {
	ba_bool paused;

	ba_audio_lock(stream->audio);
	paused = stream->paused;
	ba_audio_unlock(stream->audio);

	return paused;
}
