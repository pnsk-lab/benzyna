#include <ba_runtime.h>

static int sound_play(ba_thread_t* thread) {
	char* sound;

	if((sound = ba_thread_input(thread, "SOUND_MENU")) != NULL) {
		const char*	   f	  = shget(thread->sprite->target->sounds, sound);
		ba_audio_stream_t* stream = NULL;

		if(f != NULL) stream = ba_audio_file_open(thread->runtime->audio, f);

		if(stream != NULL) ba_audio_stream_set_autoclean(stream, ba_true);

		free(sound);
	}

	return ba_status_next;
}

static ba_bool sound_playuntildone_check(ba_thread_t* thread) {
	return !ba_audio_stream_get_paused(thread->wait.arg);
}

static void sound_playuntildone_free_arg(void* arg) {
	ba_audio_stream_free(arg);
}

static int sound_playuntildone(ba_thread_t* thread) {
	char* sound;

	if((sound = ba_thread_input(thread, "SOUND_MENU")) != NULL) {
		const char*	   f	  = shget(thread->sprite->target->sounds, sound);
		ba_audio_stream_t* stream = NULL;

		if(f != NULL) stream = ba_audio_file_open(thread->runtime->audio, f);

		if(stream != NULL) {
			thread->wait.check    = sound_playuntildone_check;
			thread->wait.arg      = stream;
			thread->wait.free_arg = sound_playuntildone_free_arg;
		}

		free(sound);
	}

	return ba_status_next;
}

void ba_block_sound(ba_runtime_t* rt) {
	ba_runtime_block_handler(rt, "sound_play", sound_play);
	ba_runtime_block_handler(rt, "sound_playuntildone", sound_playuntildone);
}
