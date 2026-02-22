#include <ba_runtime.h>

static void data_callback(ma_device* device, void* output, const void* input, ma_uint32 frame) {
	ba_audio_t*	    audio = device->pUserData;
	int		    i;
	short*		    out	    = output;
	float*		    buffer  = malloc(sizeof(*buffer) * 2 * frame);
	ba_audio_stream_t** streams = NULL;

	for(i = 0; i < 2 * frame; i++) {
		out[i] = buffer[i] = 0;
	}

	ma_mutex_lock(&audio->mutex);
	for(i = 0; i < arrlen(audio->streams); i++) {
		short* sbuffer;
		short* sbuffer2;
		short* sbuffer3;
		int    f, s;

		if(audio->streams[i]->paused && audio->streams[i]->autoclean) {
			arrput(streams, audio->streams[i]);
			continue;
		}

		if(audio->streams[i]->paused || audio->streams[i]->read == NULL || audio->streams[i]->processor == NULL) continue;

		f	= (double)frame * audio->streams[i]->rate / ba_audio_rate;
		sbuffer = malloc(sizeof(*sbuffer) * audio->streams[i]->channel * f);

		memset(sbuffer, 0, sizeof(*sbuffer) * audio->streams[i]->channel * f);
		if((s = audio->streams[i]->read(audio->streams[i], sbuffer, f)) > 0) {
			ma_uint64 fin, fout;
			int	  j;

			fin	 = f;
			fout	 = frame;
			sbuffer2 = malloc(sizeof(*sbuffer2) * audio->streams[i]->channel * fout);
			memset(sbuffer2, 0, sizeof(*sbuffer2) * audio->streams[i]->channel * fout);
			ma_resampler_process_pcm_frames(&audio->streams[i]->processor->resampler, sbuffer, &fin, sbuffer2, &fout);

			sbuffer3 = malloc(sizeof(*sbuffer3) * 2 * fout);
			memset(sbuffer3, 0, sizeof(*sbuffer3) * 2 * fout);

			ma_channel_converter_process_pcm_frames(&audio->streams[i]->processor->converter, sbuffer3, sbuffer2, fout);

			for(j = 0; j < 2 * fout; j++) {
				buffer[j] += sbuffer3[j] / 32767.0;
			}

			free(sbuffer2);
			free(sbuffer3);
		} else {
			audio->streams[i]->paused = ba_true;
		}

		free(sbuffer);
	}
	ma_mutex_unlock(&audio->mutex);

	for(i = 0; i < 2 * frame; i++) {
		if(buffer[i] < -1) buffer[i] = -1;
		if(buffer[i] > 1) buffer[i] = 1;

		out[i] = buffer[i] * 32767;
	}

	for(i = 0; i < arrlen(streams); i++) ba_audio_stream_free(streams[i]);
	arrfree(streams);
}

ba_audio_t* ba_audio_open(ba_runtime_t* rt) {
	ba_audio_t* audio = malloc(sizeof(*audio));

	memset(audio, 0, sizeof(*audio));

	audio->runtime = rt;

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
		ma_channel_converter_uninit(&stream->processor->converter, NULL);
		ma_resampler_uninit(&stream->processor->resampler, NULL);

		free(stream->processor);
	}

	free(stream);
}

void ba_audio_stream_init(ba_audio_stream_t* stream) {
	ba_audio_lock(stream->audio);
	stream->processor = malloc(sizeof(*stream->processor));

	memset(stream->processor, 0, sizeof(*stream->processor));

	stream->processor->resampler_config = ma_resampler_config_init(ma_format_s16, stream->channel, stream->rate, ba_audio_rate, ma_resample_algorithm_linear);
	ma_resampler_init(&stream->processor->resampler_config, NULL, &stream->processor->resampler);

	stream->processor->converter_config = ma_channel_converter_config_init(ma_format_s16, stream->channel, NULL, 2, NULL, ma_channel_mix_mode_default);
	ma_channel_converter_init(&stream->processor->converter_config, NULL, &stream->processor->converter);

	ba_audio_unlock(stream->audio);
}

void ba_audio_stream_set_paused(ba_audio_stream_t* stream, ba_bool paused) {
	ba_audio_lock(stream->audio);
	stream->paused = paused;
	ba_audio_unlock(stream->audio);
}

ba_bool ba_audio_stream_get_paused(ba_audio_stream_t* stream) {
	ba_bool paused;

	ba_audio_lock(stream->audio);
	paused = stream->paused;
	ba_audio_unlock(stream->audio);

	return paused;
}

void ba_audio_stream_set_autoclean(ba_audio_stream_t* stream, ba_bool autoclean) {
	ba_audio_lock(stream->audio);
	stream->autoclean = autoclean;
	ba_audio_unlock(stream->audio);
}

void ba_audio_stream_set_rate(ba_audio_stream_t* stream, int rate) {
	ba_audio_lock(stream->audio);
	stream->rate = rate;
	ba_audio_unlock(stream->audio);
}

void ba_audio_stream_set_channel(ba_audio_stream_t* stream, int channel) {
	ba_audio_lock(stream->audio);
	stream->channel = channel;
	ba_audio_unlock(stream->audio);
}
