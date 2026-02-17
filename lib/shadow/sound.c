#include <ba_runtime.h>

static char* sound_sounds_menu(ba_thread_t* thread, ba_block_t* block) {
	return ba_thread_field2(thread, block, "SOUND_MENU");
}

void ba_shadow_sound(ba_runtime_t* rt) {
	ba_runtime_shadow_handler(rt, "sound_sounds_menu", sound_sounds_menu);
}
