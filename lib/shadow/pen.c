#include <ba_runtime.h>

static char* pen_menu_colorParam(ba_thread_t* thread, ba_block_t* block) {
	return ba_thread_field2(thread, block, "colorParam");
}

void ba_shadow_pen(ba_runtime_t* rt) {
	ba_runtime_shadow_handler(rt, "pen_menu_colorParam", pen_menu_colorParam);
}
