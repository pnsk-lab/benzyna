#include <ze_runtime.h>

ze_thread_t* ze_thread_start(ze_runtime_t* rt, ze_block_t* block) {
	ze_thread_t* thread = malloc(sizeof(*thread));

	memset(thread, 0, sizeof(*thread));

	thread->block = block;

	arrput(rt->threads, thread);

	return thread;
}

void ze_thread_stop(ze_thread_t* thread) {

	thread->stopped = ze_true;
}

static ze_bool control_forever(ze_thread_t* thread) {
	return ze_true;
}

static ze_bool control_repeat(ze_thread_t* thread) {
	return ze_false;
}

void ze_thread_exec(ze_thread_t* thread) {
	int n;

	if(strcmp(thread->block->opcode, "looks_say") == 0) {
		thread->vsync = ze_true;
	} else if(strcmp(thread->block->opcode, "control_wait") == 0) {
		thread->vsync = ze_true;
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
		thread->vsync = ze_true;

		if(!thread->checkstack[n](thread)) {
			thread->block = thread->stack[n]->next;

			arrdel(thread->stack, n);
			arrdel(thread->checkstack, n);
		}
	}

recheck:;
	if(thread->block == NULL) {
		if(arrlen(thread->stack) == 0) {
			thread->stopped = ze_true;
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

void ze_thread_kill(ze_runtime_t* rt, ze_thread_t* thread) {
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
