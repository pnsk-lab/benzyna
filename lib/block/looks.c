#include <ba_runtime.h>

static int looks_say(ba_thread_t* thread) {
	char* str;

	if((str = ba_thread_input(thread, "MESSAGE")) != NULL) {
		ba_log("Says: %s", str);
		free(str);
	}

	return ba_status_next;
}

void ba_block_looks(ba_runtime_t* rt) {
	ba_runtime_block_handler(rt, "looks_say", looks_say);
}
