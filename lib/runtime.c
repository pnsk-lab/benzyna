#include <ba_runtime.h>

static ba_bool first = ba_true;

void ba_runtime_init(ba_runtime_t* rt) {
	double scale = 2;

	memset(rt, 0, sizeof(*rt));

	if(first) {
		glfwInit();
	}

	rt->window = glfwCreateWindow(BA_WIDTH * scale, BA_HEIGHT * scale, "Benzyna Scratch Runtime", NULL, NULL);

	glfwMakeContextCurrent(rt->window);

	if(first) {
		gladLoadGL();

		first = ba_false;
	}

	glfwSwapInterval(rt->turbo ? 0 : 1);

	glEnable(GL_BLEND);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glViewport(0, 0, BA_WIDTH * scale, BA_HEIGHT * scale);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-BA_WIDTH / 2, BA_WIDTH / 2, -BA_HEIGHT / 2, BA_HEIGHT / 2, -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void ba_runtime_load_project(ba_runtime_t* rt, const char* data, int size) {
	cJSON* targets;
	int    i;

	if((rt->json = cJSON_ParseWithLength(data, size)) == NULL) return;

	if((targets = cJSON_GetObjectItem(rt->json, "targets")) == NULL || targets->type != cJSON_Array) return;

	ba_log("%d target(s)", cJSON_GetArraySize(targets));

	targets = targets->child;
	while(targets != NULL) {
		ba_target_t* t = ba_target_parse(rt, targets);
		if(t == NULL) {
			return;
		}

		arrput(rt->targets, t);

		targets = targets->next;
	}

	for(i = 0; i < arrlen(rt->targets); i++) {
		int	     j;
		ba_sprite_t* spr;
		cJSON*	     js;

		spr = ba_sprite_start(rt, rt->targets[i]);

		js = cJSON_GetObjectItem(rt->targets[i]->json, "currentCostume");
		if(js != NULL && js->type == cJSON_Number) spr->costume = js->valuedouble;
		js = cJSON_GetObjectItem(rt->targets[i]->json, "x");
		if(js != NULL && js->type == cJSON_Number) spr->x = js->valuedouble;
		js = cJSON_GetObjectItem(rt->targets[i]->json, "y");
		if(js != NULL && js->type == cJSON_Number) spr->y = js->valuedouble;
		js = cJSON_GetObjectItem(rt->targets[i]->json, "direction");
		if(js != NULL && js->type == cJSON_Number) {
			spr->angle = js->valuedouble;

			if(spr->angle < 0) spr->angle = 180 + spr->angle;
			spr->angle -= 90;
		}

		for(j = 0; j < arrlen(rt->targets[i]->tree); j++) {
			if(strcmp(rt->targets[i]->tree[j]->opcode, "event_whenflagclicked") == 0) {
				ba_thread_t* thread = ba_thread_start(rt, rt->targets[i]->tree[j]);

				thread->sprite = spr;
			}
		}
	}
}

void ba_runtime_loop(ba_runtime_t* rt) {
	while(!glfwWindowShouldClose(rt->window)) {
		int	i;
		ba_bool loop;

		do {
			loop = ba_false;
			for(i = 0; i < arrlen(rt->threads); i++) {
				if(rt->threads[i]->vsync || rt->threads[i]->stopped) continue;
				loop = ba_true;

				ba_thread_exec(rt->threads[i]);
			}
		} while(loop);

		for(i = 0; i < arrlen(rt->threads); i++) {
			rt->threads[i]->vsync = ba_false;
			if(rt->threads[i]->stopped) {
				ba_thread_kill(rt, rt->threads[i]);
				i--;
			}
		}

		ba_render(rt);

		glfwPollEvents();
	}
}

void ba_runtime_uninit(ba_runtime_t* rt) {
	int i;
	for(i = 0; i < arrlen(rt->threads); i++) {
		ba_thread_kill(rt, rt->threads[i]);
		i--;
	}
	arrfree(rt->threads);

	for(i = 0; i < arrlen(rt->sprites); i++) ba_sprite_kill(rt, rt->sprites[i]);
	arrfree(rt->sprites);

	for(i = 0; i < arrlen(rt->targets); i++) ba_target_free(rt->targets[i]);
	arrfree(rt->targets);

	if(rt->json != NULL) cJSON_Delete(rt->json);
}

ba_sprite_t* ba_runtime_get_stage_sprite(ba_runtime_t* rt) {
	int i;

	for(i = 0; i < arrlen(rt->sprites); i++) {
		if(rt->sprites[i]->target->stage) return rt->sprites[i];
	}

	return NULL;
}
