#include <ba_runtime.h>

static int control_wait(ba_thread_t* thread) {
	thread->vsync = ba_true;

	return ba_status_next;
}

static ba_bool control_forever_check(ba_thread_t* thread) {
	return ba_true;
}

static int control_forever(ba_thread_t* thread) {
	arrput(thread->stack, thread->block);
	arrput(thread->checkstack, control_forever_check);
	thread->block = thread->block->children;

	return ba_status_stay;
}

static ba_bool control_repeat_check(ba_thread_t* thread) {
	return ba_false;
}

static int control_repeat(ba_thread_t* thread) {
	arrput(thread->stack, thread->block);
	arrput(thread->checkstack, control_repeat_check);
	thread->block = thread->block->children;

	return ba_status_stay;
}

void ba_block_control(ba_runtime_t* rt) {
	ba_runtime_block_handler(rt, "control_wait", control_wait);
	ba_runtime_block_handler(rt, "control_forever", control_forever);
	ba_runtime_block_handler(rt, "control_repeat", control_repeat);
}
