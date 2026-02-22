#include <ba_runtime.h>

static void pen_new(ba_thread_t* thread, ba_sprite_t* sprite) {
	if(sprite == NULL) sprite = thread->sprite;

	sprite->pen = ba_runtime_pen(thread->runtime);

	memcpy(sprite->pen->color, sprite->pen_color, sizeof(double) * 3);
	arrput(sprite->pen->coords, sprite->x);
	arrput(sprite->pen->coords, sprite->y);
}

static int pen_penDown(ba_thread_t* thread) {
	if(thread->sprite->pen != NULL) return ba_status_next;

	pen_new(thread, NULL);

	return ba_status_next;
}

static int pen_penUp(ba_thread_t* thread) {
	thread->sprite->pen = NULL;

	return ba_status_next;
}

static int pen_changePenColorParamBy(ba_thread_t* thread) {
	char* param = NULL;
	char* value = NULL;

	if((param = ba_thread_input(thread, "COLOR_PARAM")) != NULL && (value = ba_thread_input(thread, "VALUE")) != NULL) {
		int i;

		if(strcmp(param, "color") == 0) {
			thread->sprite->pen_color[0] += atof(value) / 100;
		} else if(strcmp(param, "saturation") == 0) {
			thread->sprite->pen_color[1] += atof(value) / 100;
		} else if(strcmp(param, "brightness") == 0) {
			thread->sprite->pen_color[2] += atof(value) / 100;
		}

		for(i = 0; i < 3; i++) {
			if(thread->sprite->pen_color[i] < 0) thread->sprite->pen_color[i] = 0;
			if(thread->sprite->pen_color[i] > 1) thread->sprite->pen_color[i] = 1;
		}

		if(thread->sprite->pen != NULL) pen_new(thread, NULL);
	}

	if(param != NULL) free(param);
	if(value != NULL) free(value);

	return ba_status_next;
}

static int pen_changePenColorParamTo(ba_thread_t* thread) {
	char* param = NULL;
	char* value = NULL;

	if((param = ba_thread_input(thread, "COLOR_PARAM")) != NULL && (value = ba_thread_input(thread, "VALUE")) != NULL) {
		int i;

		if(strcmp(param, "color") == 0) {
			thread->sprite->pen_color[0] = atof(value) / 100;
		} else if(strcmp(param, "saturation") == 0) {
			thread->sprite->pen_color[1] = atof(value) / 100;
		} else if(strcmp(param, "brightness") == 0) {
			thread->sprite->pen_color[2] = atof(value) / 100;
		}

		for(i = 0; i < 3; i++) {
			if(thread->sprite->pen_color[i] < 0) thread->sprite->pen_color[i] = 0;
			if(thread->sprite->pen_color[i] > 1) thread->sprite->pen_color[i] = 1;
		}

		if(thread->sprite->pen != NULL) pen_new(thread, NULL);
	}

	if(param != NULL) free(param);
	if(value != NULL) free(value);

	return ba_status_next;
}

static int pen_clear(ba_thread_t* thread) {
	int	      i;
	ba_sprite_t** sprites = NULL;

	for(i = 0; i < arrlen(thread->runtime->sprites); i++) {
		if(thread->runtime->sprites[i]->pen != NULL) arrput(sprites, thread->runtime->sprites[i]);
	}

	for(i = 0; i < arrlen(thread->runtime->pens); i++) {
		arrfree(thread->runtime->pens[i]->coords);
		free(thread->runtime->pens[i]);
	}
	arrfree(thread->runtime->pens);

	for(i = 0; i < arrlen(sprites); i++) {
		pen_new(thread, sprites[i]);
	}

	arrfree(sprites);

	return ba_status_next;
}

void ba_block_pen(ba_runtime_t* rt) {
	ba_runtime_block_handler(rt, "pen_penDown", pen_penDown);
	ba_runtime_block_handler(rt, "pen_penUp", pen_penUp);
	ba_runtime_block_handler(rt, "pen_changePenColorParamBy", pen_changePenColorParamBy);
	ba_runtime_block_handler(rt, "pen_changePenColorParamTo", pen_changePenColorParamTo);
	ba_runtime_block_handler(rt, "pen_clear", pen_clear);
}
