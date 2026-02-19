#include <ba_runtime.h>

static ba_bool text2speech_speakAndWait_check(ba_thread_t* thread) {
	return !ba_audio_stream_get_paused(thread->wait.arg);
}

static void text2speech_speakAndWait_free_arg(void* arg) {
	ba_audio_stream_free(arg);
}

static int text2speech_speakAndWait_read(ba_audio_stream_t* stream, short* buffer, int frame) {
	short* buf = stream->opaque1;
	int*   arr = stream->opaque2;
	int    f   = frame > (arr[1] - arr[0]) ? (arr[1] - arr[0]) : frame;

	memcpy(buffer, buf + arr[0], f * 2);

	arr[0] += f;

	return f;
}

static void text2speech_speakAndWait_free(ba_audio_stream_t* stream) {
	speech_free(stream->opaque1, NULL);
	free(stream->opaque2);
}

static int text2speech_speakAndWait(ba_thread_t* thread) {
	char* words;

	if((words = ba_thread_input(thread, "WORDS")) != NULL) {
		ba_audio_stream_t* stream = NULL;
		int*		   arr	  = malloc(sizeof(*arr) * 2);

		arr[0] = 0;

		stream = ba_audio_stream_new(thread->runtime->audio);

		ba_audio_lock(thread->runtime->audio);
		stream->opaque1 = speech_gen(&arr[1], words, NULL);
		stream->opaque2 = arr;
		stream->read	= text2speech_speakAndWait_read;
		stream->free	= text2speech_speakAndWait_free;
		ba_audio_unlock(thread->runtime->audio);

		ba_audio_stream_set_rate(stream, 11025);
		ba_audio_stream_set_channel(stream, 1);
		ba_audio_stream_init(stream);
		ba_audio_stream_set_paused(stream, ba_false);

		if(stream != NULL) {
			thread->wait.check    = text2speech_speakAndWait_check;
			thread->wait.arg      = stream;
			thread->wait.free_arg = text2speech_speakAndWait_free_arg;
		}

		free(words);
	}

	return ba_status_next;
}

void ba_block_text2speech(ba_runtime_t* rt) {
	ba_runtime_block_handler(rt, "text2speech_speakAndWait", text2speech_speakAndWait);
}
