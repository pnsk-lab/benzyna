#include <ba_runtime.h>

static ba_bool control_forever(ba_thread_t* thread) {
	return ba_true;
}

static ba_bool control_repeat(ba_thread_t* thread) {
	return ba_false;
}

int ba_block_control(ba_thread_t* thread) {
	if(strcmp(thread->block->opcode, "control_wait") == 0) {
		thread->vsync = ba_true;

		return ba_status_next;
	} else if(strcmp(thread->block->opcode, "control_forever") == 0) {
		arrput(thread->stack, thread->block);
		arrput(thread->checkstack, control_forever);
		thread->block = thread->block->children;
	} else if(strcmp(thread->block->opcode, "control_repeat") == 0) {
		arrput(thread->stack, thread->block);
		arrput(thread->checkstack, control_repeat);
		thread->block = thread->block->children;
	} else {
		return ba_status_declined;
	}

	return ba_status_ok;
}
