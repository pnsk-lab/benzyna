#include <ba_runtime.h>

static int pen_penDown(ba_thread_t* thread) {
	if(thread->sprite->pen != NULL) return ba_status_next;

	thread->sprite->pen = ba_runtime_pen(thread->runtime);

	memcpy(thread->sprite->pen->color, thread->sprite->pen_color, sizeof(double) * 3);
	arrput(thread->sprite->pen->coords, thread->sprite->x);
	arrput(thread->sprite->pen->coords, thread->sprite->y);

	return ba_status_next;
}

static int pen_penUp(ba_thread_t* thread) {
	thread->sprite->pen = NULL;

	return ba_status_next;
}

void ba_block_pen(ba_runtime_t* rt) {
	ba_runtime_block_handler(rt, "pen_penDown", pen_penDown);
	ba_runtime_block_handler(rt, "pen_penUp", pen_penUp);
}
