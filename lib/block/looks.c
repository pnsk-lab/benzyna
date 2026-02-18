#include <ba_runtime.h>

static int looks_say(ba_thread_t* thread) {
	char* str;

	if((str = ba_thread_input(thread, "MESSAGE")) != NULL) {
		ba_log("Says: %s", str);
		free(str);
	}

	return ba_status_next;
}

static int looks_think(ba_thread_t* thread) {
	char* str;

	if((str = ba_thread_input(thread, "MESSAGE")) != NULL) {
		ba_log("Thinks: %s", str);
		free(str);
	}

	return ba_status_next;
}

static int looks_nextcostume(ba_thread_t* thread) {
	thread->sprite->costume++;
	thread->sprite->costume = thread->sprite->costume % arrlen(thread->sprite->target->costumes);

	return ba_status_next;
}

static int looks_nextbackdrop(ba_thread_t* thread) {
	ba_sprite_t* sprite = ba_runtime_get_stage_sprite(thread->runtime);

	if(sprite != NULL) {
		sprite->costume++;
		sprite->costume = sprite->costume % arrlen(sprite->target->costumes);
	}

	return ba_status_next;
}

void ba_block_looks(ba_runtime_t* rt) {
	ba_runtime_block_handler(rt, "looks_say", looks_say);
	ba_runtime_block_handler(rt, "looks_think", looks_think);
	ba_runtime_block_handler(rt, "looks_nextcostume", looks_nextcostume);
	ba_runtime_block_handler(rt, "looks_nextbackdrop", looks_nextbackdrop);
}
