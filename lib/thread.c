#include <ba_runtime.h>

ba_thread_t* ba_thread_start(ba_runtime_t* rt, ba_block_t* block) {
	ba_thread_t* thread = malloc(sizeof(*thread));

	memset(thread, 0, sizeof(*thread));

	thread->block = block;

	thread->runtime = rt;

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
	int		   st;
	ba_block_handler_t handler;

	st = ba_status_next;
	if((handler = shget(thread->runtime->block_handlers, thread->block->opcode)) != NULL) {
		st = handler(thread);
	}

	if(st == ba_status_next) {
		thread->block = thread->block->next;
	}

recheck:;
	if(thread->block == NULL) {
		if(arrlen(thread->loopstack) == 0) {
			thread->stopped = ba_true;
		} else {
			int n = arrlen(thread->loopstack) - 1;

			if(thread->checkstack[n] != NULL && thread->checkstack[n](thread)) {
				thread->block = thread->loopstack[n];
			} else {
				thread->block = thread->escstack[n];

				if(thread->argstack[n] != NULL) free(thread->argstack[n]);

				arrdel(thread->loopstack, n);
				arrdel(thread->escstack, n);
				arrdel(thread->checkstack, n);
				arrdel(thread->argstack, n);
			}
			thread->vsync = ba_true;

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

	for(i = 0; i < arrlen(thread->argstack); i++) {
		if(thread->argstack[i] != NULL) free(thread->argstack[i]);
	}

	arrfree(thread->checkstack);
	arrfree(thread->loopstack);
	arrfree(thread->escstack);
	free(thread);
}

char* ba_thread_input(ba_thread_t* thread, const char* input) {
	return ba_thread_input2(thread, thread->block, input);
}

char* ba_thread_input2(ba_thread_t* thread, ba_block_t* block, const char* input) {
	int ind = shgeti(block->inputs, input);

	if(ind == -1) return NULL;

	return ba_exec(thread, &block->inputs[ind].value);
}
