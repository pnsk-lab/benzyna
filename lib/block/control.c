#include <ba_runtime.h>

static ba_bool control_wait_check(ba_thread_t* thread) {
	double* n = thread->wait.arg;

	return (*n) >= ba_time_tick();
}

static int control_wait(ba_thread_t* thread) {
	double* n = malloc(sizeof(*n));
	char*	str;

	if((str = ba_thread_input(thread, "DURATION")) != NULL) {
		*n = ba_time_tick() + atof(str);

		memset(&thread->wait, 0, sizeof(thread->wait));

		thread->wait.check = control_wait_check;
		thread->wait.arg   = n;

		free(str);
	}

	return ba_status_next;
}

static ba_bool control_forever_check(ba_thread_t* thread) {
	return ba_true;
}

static int control_forever(ba_thread_t* thread) {
	ba_thread_stack_t e;

	memset(&e, 0, sizeof(e));

	e.loop	= thread->block->children;
	e.check = control_forever_check;
	arrput(thread->stack, e);

	thread->block = thread->block->children;

	return ba_status_stay;
}

static ba_bool control_repeat_check(ba_thread_t* thread) {
	int* n = thread->stack[arrlen(thread->stack) - 1].arg;

	(*n)--;

	return (*n) > 0;
}

static int control_repeat(ba_thread_t* thread) {
	int*		  n = malloc(sizeof(*n));
	char*		  str;
	ba_thread_stack_t e;

	memset(&e, 0, sizeof(e));

	*n = 0;

	if((str = ba_thread_input(thread, "TIMES")) != NULL) {
		*n = atof(str);

		free(str);
	}

	if((*n) == 0) return ba_status_next;

	e.loop	 = thread->block->children;
	e.escape = thread->block->next;
	e.check	 = control_repeat_check;
	e.arg	 = n;
	arrput(thread->stack, e);

	thread->block = thread->block->children;

	return ba_status_stay;
}

static int control_if_else(ba_thread_t* thread) {
	ba_input_t*	  substack = NULL;
	ba_block_t*	  block;
	char*		  str = NULL;
	ba_thread_stack_t e;

	memset(&e, 0, sizeof(e));

	if((str = ba_thread_input(thread, "CONDITION")) != NULL && ba_string_is_false(str)) {
		substack = ba_block_input(thread->block, "SUBSTACK");
	} else {
		substack = ba_block_input(thread->block, "SUBSTACK2");
	}

	if(str != NULL) free(str);

	if(substack == NULL || substack->type != ba_input_block || (block = shget(thread->sprite->target->blocks, substack->u.block)) == NULL) return ba_status_next;

	e.escape = thread->block->next;
	arrput(thread->stack, e);

	thread->block = block;

	return ba_status_stay;
}

static int control_if(ba_thread_t* thread) {
	ba_block_t*	  block = NULL;
	char*		  str	= NULL;
	ba_thread_stack_t e;

	memset(&e, 0, sizeof(e));

	if((str = ba_thread_input(thread, "CONDITION")) != NULL && ba_string_is_false(str)) {
		block = thread->block->children;
	}

	if(str != NULL) free(str);

	if(block == NULL) return ba_status_next;

	e.escape = thread->block->next;
	arrput(thread->stack, e);

	thread->block = block;

	return ba_status_stay;
}

void ba_block_control(ba_runtime_t* rt) {
	ba_runtime_block_handler(rt, "control_wait", control_wait);
	ba_runtime_block_handler(rt, "control_forever", control_forever);
	ba_runtime_block_handler(rt, "control_repeat", control_repeat);
	ba_runtime_block_handler(rt, "control_if_else", control_if_else);
	ba_runtime_block_handler(rt, "control_if", control_if);
}
