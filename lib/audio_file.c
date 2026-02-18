#include <ba_runtime.h>

#define DR(name) \
	static int name##_read(ba_audio_stream_t* stream, short* buffer, int frame) { \
		return dr##name##_read_pcm_frames_s16(stream->opaque1, frame, buffer); \
	} \
\
	static void name##_free(ba_audio_stream_t* stream) { \
		dr##name##_uninit(stream->opaque1); \
		free(stream->opaque1); \
		free(stream->opaque2); \
	} \
\
	static ba_audio_stream_t* open_##name(ba_audio_t* audio, unsigned char* buffer, int size) { \
		ba_audio_stream_t* stream; \
		dr##name*	   h = malloc(sizeof(*h)); \
\
		if(!dr##name##_init_memory(h, buffer, size, NULL)) { \
			free(h); \
\
			return NULL; \
		} \
\
		stream = ba_audio_stream_new(audio); \
\
		ba_audio_lock(audio); \
		stream->opaque1 = h; \
		stream->opaque2 = buffer; \
		stream->read	= name##_read; \
		stream->free	= name##_free; \
		ba_audio_unlock(audio); \
\
		ba_audio_stream_set_rate(stream, h->sampleRate); \
		ba_audio_stream_set_channel(stream, h->channels); \
		ba_audio_stream_init(stream); \
		ba_audio_stream_set_paused(stream, ba_false); \
\
		return stream; \
	}

DR(mp3);
DR(wav);

ba_audio_stream_t* ba_audio_file_open(ba_audio_t* audio, const char* path) {
	int		   size;
	unsigned char*	   buf;
	ba_audio_stream_t* stream = NULL;

	if((buf = audio->runtime->param.load_file(audio->runtime, path, &size)) != NULL) {
		if((size > 2 && buf[0] == 0xff && (buf[1] == 0xfb || buf[1] == 0xf3 || buf[2] == 0xf2)) || (size > 3 && memcmp(buf, "ID3", 3) == 0)) {
			stream = open_mp3(audio, buf, size);
		} else if(size > 4 && memcmp(buf, "RIFF", 4) == 0) {
			stream = open_wav(audio, buf, size);
		} else {
			free(buf);
			return NULL;
		}
	}

	return stream;
}
