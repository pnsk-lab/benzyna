#include <ze_runtime.h>

ze_target_t* ze_target_parse(ze_runtime_t* rt, ze_cJSON* json) {
	ze_target_t* target = malloc(sizeof(*target));
	cJSON*	     costumes;

	memset(target, 0, sizeof(*target));

	if((costumes = cJSON_GetObjectItem(json, "costumes")) == NULL || costumes->type != cJSON_Array) {
		ze_target_free(target);
		return NULL;
	}

	costumes = costumes->child;
	while(costumes != NULL) {
		ze_costume_t* c = ze_costume_parse(rt, costumes);
		if(c == NULL) {
			ze_target_free(target);
			return NULL;
		}

		arrput(target->costumes, c);

		costumes = costumes->next;
	}

	return target;
}

void ze_target_free(ze_target_t* target) {
	int i;
	for(i = 0; i < arrlen(target->costumes); i++) ze_costume_free(target->costumes[i]);

	free(target);
}
