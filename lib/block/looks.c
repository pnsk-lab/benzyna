#include <ba_runtime.h>

int ba_block_looks(ba_thread_t* thread) {
	if(strcmp(thread->block->opcode, "looks_say") == 0) {
		const char* s	= "MESSAGE";
		int	    ind = shgeti(thread->block->inputs, s);

		if(ind != -1) {
			char* str;

			if((str = ba_exec(thread, &thread->block->inputs[ind].value)) != NULL) {
				ba_log("Says: %s", str);
				free(str);
			}
		}
	} else {
		return ba_status_declined;
	}

	return ba_status_next;
}
