#include <ze_runtime.h>

static ze_bool first = ze_true;

void ze_runtime_init(ze_runtime_t* rt) {
	double scale = 2;

	memset(rt, 0, sizeof(*rt));

	if(first) {
		glfwInit();
	}

	rt->window = glfwCreateWindow(ZE_WIDTH * scale, ZE_HEIGHT * scale, "Zadrapanie", NULL, NULL);

	glfwMakeContextCurrent(rt->window);

	if(first) {
		gladLoadGL();

		first = ze_false;
	}

	glfwSwapInterval(rt->turbo ? 0 : 1);

	glEnable(GL_BLEND);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glViewport(0, 0, ZE_WIDTH * scale, ZE_HEIGHT * scale);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-ZE_WIDTH / 2, ZE_WIDTH / 2, -ZE_HEIGHT / 2, ZE_HEIGHT / 2, -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void ze_runtime_load_project(ze_runtime_t* rt, const char* data, int size) {
	cJSON* targets;
	int    i;

	if((rt->json = cJSON_ParseWithLength(data, size)) == NULL) return;

	if((targets = cJSON_GetObjectItem(rt->json, "targets")) == NULL || targets->type != cJSON_Array) return;

	ze_log("%d target(s)", cJSON_GetArraySize(targets));

	targets = targets->child;
	while(targets != NULL) {
		ze_target_t* t = ze_target_parse(rt, targets);
		if(t == NULL) {
			return;
		}

		arrput(rt->targets, t);

		targets = targets->next;
	}

	for(i = 0; i < arrlen(rt->targets); i++) {
		int	     j;
		ze_sprite_t* spr;
		cJSON*	     js;

		spr = ze_sprite_start(rt, rt->targets[i], ze_false);

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
				ze_thread_t* thread = ze_thread_start(rt, rt->targets[i]->tree[j]);

				thread->sprite = spr;
			}
		}
	}
}

void ze_runtime_loop(ze_runtime_t* rt) {
	while(!glfwWindowShouldClose(rt->window)) {
		int	i;
		ze_bool loop;

		do {
			loop = ze_false;
			for(i = 0; i < arrlen(rt->threads); i++) {
				if(rt->threads[i]->vsync || rt->threads[i]->stopped) continue;
				loop = ze_true;

				ze_thread_exec(rt->threads[i]);
			}
		} while(loop);

		for(i = 0; i < arrlen(rt->threads); i++) {
			rt->threads[i]->vsync = ze_false;
			if(rt->threads[i]->stopped) {
				ze_thread_kill(rt, rt->threads[i]);
				i--;
			}
		}

		ze_render(rt);

		glfwPollEvents();
	}
}

void ze_runtime_uninit(ze_runtime_t* rt) {
	int i;
	for(i = 0; i < arrlen(rt->threads); i++) {
		ze_thread_kill(rt, rt->threads[i]);
		i--;
	}
	arrfree(rt->targets);

	for(i = 0; i < arrlen(rt->targets); i++) ze_target_free(rt->targets[i]);
	arrfree(rt->targets);

	if(rt->json != NULL) cJSON_Delete(rt->json);
}
