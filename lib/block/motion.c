#include <ba_runtime.h>

static int motion_movesteps(ba_thread_t* thread) {
	char* str;

	if((str = ba_thread_input(thread, "STEPS")) != NULL) {
		double c = cos(thread->sprite->angle / 180 * M_PI);
		double s = sin(thread->sprite->angle / 180 * M_PI);

		thread->sprite->x += c * atof(str);
		thread->sprite->y += s * atof(str);

		free(str);
	}

	thread->vsync = ba_true;

	return ba_status_next;
}

static int motion_turnleft(ba_thread_t* thread) {
	char* str;

	if((str = ba_thread_input(thread, "DEGREES")) != NULL) {
		thread->sprite->angle += atof(str);
		free(str);
	}

	return ba_status_next;
}

static int motion_turnright(ba_thread_t* thread) {
	char* str;

	if((str = ba_thread_input(thread, "DEGREES")) != NULL) {
		thread->sprite->angle += -atof(str);
		free(str);
	}

	return ba_status_next;
}

static int motion_pointindirection(ba_thread_t* thread) {
	char* str;

	if((str = ba_thread_input(thread, "DIRECTION")) != NULL) {
		thread->sprite->angle += atof(str);
		free(str);
	}

	return ba_status_next;
}

static int motion_gotoxy(ba_thread_t* thread) {
	char* str1 = NULL;
	char* str2 = NULL;

	if((str1 = ba_thread_input(thread, "X")) != NULL && (str2 = ba_thread_input(thread, "Y")) != NULL) {
		thread->sprite->x = atof(str1);
		thread->sprite->y = atof(str2);
	}

	if(str1 != NULL) free(str1);
	if(str2 != NULL) free(str2);

	return ba_status_next;
}

void ba_block_motion(ba_runtime_t* rt) {
	ba_runtime_block_handler(rt, "motion_movesteps", motion_movesteps);
	ba_runtime_block_handler(rt, "motion_turnleft", motion_turnleft);
	ba_runtime_block_handler(rt, "motion_turnright", motion_turnright);
	ba_runtime_block_handler(rt, "motion_pointindirection", motion_pointindirection);
	ba_runtime_block_handler(rt, "motion_gotoxy", motion_gotoxy);
}
