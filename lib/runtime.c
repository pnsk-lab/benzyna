#include <ze_runtime.h>

static ze_bool first = ze_true;

void ze_runtime_init(ze_runtime_t* rt) {
	memset(rt, 0, sizeof(*rt));

	if(first) {
		glfwInit();
	}

	rt->window = glfwCreateWindow(480, 360, "Zadrapanie", NULL, NULL);

	if(first) {
		glfwMakeContextCurrent(rt->window);
		gladLoadGL();

		first = ze_false;
	}
}

void ze_runtime_load_project(ze_runtime_t* rt, const char* data, int size) {
	cJSON* targets;

	if((rt->root = cJSON_ParseWithLength(data, size)) == NULL) return;

	if((targets = cJSON_GetObjectItem(rt->root, "targets")) == NULL || targets->type != cJSON_Array) return;

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

	if(rt->root != NULL) cJSON_Delete(rt->root);
}
