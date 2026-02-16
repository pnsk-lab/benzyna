#include <ba_runtime.h>

static int control_wait(ba_thread_t* thread) {
	thread->vsync = ba_true;

	return ba_status_next;
}

static ba_bool control_forever_check(ba_thread_t* thread) {
	return ba_true;
}

static int control_forever(ba_thread_t* thread) {
	arrput(thread->loopstack, thread->block->children);
	arrput(thread->escstack, NULL);
	arrput(thread->checkstack, control_forever_check);
	arrput(thread->argstack, NULL);
	thread->block = thread->block->children;

	return ba_status_stay;
}

static ba_bool control_repeat_check(ba_thread_t* thread) {
	int* n = thread->argstack[arrlen(thread->argstack) - 1];

	(*n)--;

	return (*n) > 0;
}

static int control_repeat(ba_thread_t* thread) {
	int*  n = malloc(sizeof(*n));
	char* str;

	*n = 0;

	if((str = ba_thread_input(thread, "TIMES")) != NULL) {
		*n = atof(str);

		free(str);
	}

	if((*n) == 0) return ba_status_next;

	arrput(thread->loopstack, thread->block->children);
	arrput(thread->escstack, thread->block->next);
	arrput(thread->checkstack, control_repeat_check);
	arrput(thread->argstack, n);
	thread->block = thread->block->children;

	return ba_status_stay;
}

static ba_bool control_if_else_check(ba_thread_t* thread) {
	return ba_false;
}

static int control_if_else(ba_thread_t* thread) {
	ba_input_t* substack = NULL;
	ba_block_t* block;
	char*	    str = NULL;

	if((str = ba_thread_input(thread, "CONDITION")) != NULL && ba_string_is_false(str)) {
		substack = ba_block_input(thread->block, "SUBSTACK");
	} else {
		substack = ba_block_input(thread->block, "SUBSTACK2");
	}

	if(str != NULL) free(str);

	if(substack == NULL || substack->type != ba_input_block || (block = shget(thread->sprite->target->blocks, substack->u.block)) == NULL) return ba_status_next;

	arrput(thread->loopstack, NULL);
	arrput(thread->escstack, thread->block->next);
	arrput(thread->checkstack, control_if_else_check);
	arrput(thread->argstack, NULL);
	thread->block = block;

	return ba_status_stay;
}

void ba_block_control(ba_runtime_t* rt) {
	ba_runtime_block_handler(rt, "control_wait", control_wait);
	ba_runtime_block_handler(rt, "control_forever", control_forever);
	ba_runtime_block_handler(rt, "control_repeat", control_repeat);
	ba_runtime_block_handler(rt, "control_if_else", control_if_else);
}
