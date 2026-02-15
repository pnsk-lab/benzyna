#include <ba_runtime.h>

ba_thread_t* ba_thread_start(ba_runtime_t* rt, ba_block_t* block) {
	ba_thread_t* thread = malloc(sizeof(*thread));

	memset(thread, 0, sizeof(*thread));

	thread->block = block;

	arrput(rt->threads, thread);

	return thread;
}

void ba_thread_stop(ba_thread_t* thread) {

	thread->stopped = ba_true;
}

static ba_bool control_forever(ba_thread_t* thread) {
	return ba_true;
}

static ba_bool control_repeat(ba_thread_t* thread) {
	return ba_false;
}

void ba_thread_exec(ba_thread_t* thread) {
	int n;

	if(strcmp(thread->block->opcode, "looks_say") == 0) {
		thread->vsync = ba_true;
	} else if(strcmp(thread->block->opcode, "control_wait") == 0) {
		thread->vsync = ba_true;
	}

	n = arrlen(thread->stack);
	if(strcmp(thread->block->opcode, "control_forever") == 0) {
		arrput(thread->stack, thread->block);
		arrput(thread->checkstack, control_forever);
		thread->block = thread->block->children;
	} else if(strcmp(thread->block->opcode, "control_repeat") == 0) {
		arrput(thread->stack, thread->block);
		arrput(thread->checkstack, control_repeat);
		thread->block = thread->block->children;
	} else {
		thread->block = thread->block->next;
	}

	if(n != arrlen(thread->stack)) {
		n	      = arrlen(thread->stack) - 1;
		thread->vsync = ba_true;

		if(!thread->checkstack[n](thread)) {
			thread->block = thread->stack[n]->next;

			arrdel(thread->stack, n);
			arrdel(thread->checkstack, n);
		}
	}

recheck:;
	if(thread->block == NULL) {
		if(arrlen(thread->stack) == 0) {
			thread->stopped = ba_true;
		} else {
			n = arrlen(thread->stack) - 1;

			thread->block = thread->stack[n];

			if(!thread->checkstack[n](thread)) {
				thread->block = thread->block->next;
			}

			arrdel(thread->stack, n);
			arrdel(thread->checkstack, n);

			goto recheck;
		}
	}
}

void ba_thread_kill(ba_runtime_t* rt, ba_thread_t* thread) {
	int i;

	for(i = 0; i < arrlen(rt->threads); i++) {
		if(rt->threads[i] == thread) {
			arrdel(rt->threads, i);
			break;
		}
	}

	arrfree(thread->checkstack);
	arrfree(thread->stack);
	free(thread);
}
