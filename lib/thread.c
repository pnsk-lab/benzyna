#include <ba_runtime.h>

ba_thread_t* ba_thread_start(ba_runtime_t* rt, ba_block_t* block) {
	ba_thread_t* thread = malloc(sizeof(*thread));

	memset(thread, 0, sizeof(*thread));

	thread->block = block;

	thread->runtime = rt;

	thread->autoclean = ba_false;

	arrput(rt->threads, thread);

	return thread;
}

void ba_thread_stop(ba_thread_t* thread) {
	thread->stopped	  = ba_true;
	thread->autoclean = ba_true;
}

void ba_thread_step(ba_thread_t* thread) {
	int			  st;
	ba_thread_block_handler_t handler;

	st = ba_status_next;
	if((handler = shget(thread->runtime->block_handlers, thread->block->opcode)) != NULL) {
		st = handler(thread);
	}

	if(st == ba_status_next) {
		thread->block = thread->block->next;
	}

recheck:;
	if(thread->block == NULL) {
		if(arrlen(thread->stack) == 0) {
			thread->stopped = ba_true;
		} else {
			int n = arrlen(thread->stack) - 1;

			if(thread->stack[n].check != NULL && thread->stack[n].check(thread)) {
				thread->block = thread->stack[n].loop;
			} else {
				thread->block = thread->stack[n].escape;

				if(thread->stack[n].arg != NULL) (thread->stack[n].free_arg != NULL ? thread->stack[n].free_arg : free)(thread->stack[n].arg);

				arrdel(thread->stack, n);
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

	for(i = 0; i < arrlen(thread->stack); i++) {
		if(thread->stack[i].arg != NULL) (thread->stack[i].free_arg != NULL ? thread->stack[i].free_arg : free)(thread->stack[i].arg);
	}
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

char* ba_thread_field(ba_thread_t* thread, const char* field) {
	return ba_thread_field2(thread, thread->block, field);
}

char* ba_thread_field2(ba_thread_t* thread, ba_block_t* block, const char* field) {
	int ind = shgeti(block->fields, field);

	if(ind == -1) return NULL;

	return ba_string_dup(block->fields[ind].value);
}
