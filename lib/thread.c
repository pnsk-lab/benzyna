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
	int		   n;
	ba_block_handler_t handler;

	n  = arrlen(thread->stack);
	st = ba_status_next;
	if((handler = shget(thread->runtime->block_handlers, thread->block->opcode)) != NULL) {
		st = handler(thread);
	}

	if(st == ba_status_next) {
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

char* ba_thread_input(ba_thread_t* thread, const char* input) {
	return ba_thread_input2(thread, thread->block, input);
}

char* ba_thread_input2(ba_thread_t* thread, ba_block_t* block, const char* input) {
	int ind = shgeti(block->inputs, input);

	if(ind == -1) return NULL;

	return ba_exec(thread, &block->inputs[ind].value);
}
