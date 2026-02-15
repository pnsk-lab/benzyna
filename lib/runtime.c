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
}

void ze_runtime_loop(ze_runtime_t* rt) {
	while(!glfwWindowShouldClose(rt->window)) {
		ze_render(rt);

		glfwPollEvents();
	}
}

void ze_runtime_uninit(ze_runtime_t* rt) {
	int i;
	for(i = 0; i < arrlen(rt->targets); i++) ze_target_free(rt->targets[i]);
	arrfree(rt->targets);

	if(rt->json != NULL) cJSON_Delete(rt->json);
}
