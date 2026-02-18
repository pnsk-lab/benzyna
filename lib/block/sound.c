#include <ba_runtime.h>

static int sound_play(ba_thread_t* thread) {
	char* sound;

	if((sound = ba_thread_input(thread, "SOUND_MENU")) != NULL) {
		free(sound);
	}

	return ba_status_next;
}

void ba_block_sound(ba_runtime_t* rt) {
	ba_runtime_block_handler(rt, "sound_play", sound_play);
}
