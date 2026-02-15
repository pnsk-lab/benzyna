#include <ze_runtime.h>

ze_target_t* ze_target_parse(ze_runtime_t* rt, ze_cJSON* json) {
	ze_target_t* target = malloc(sizeof(*target));
	cJSON*	     costumes;
	cJSON*	     blocks;
	int	     i;

	memset(target, 0, sizeof(*target));

	target->json = json;

	if((costumes = cJSON_GetObjectItem(json, "costumes")) == NULL || costumes->type != cJSON_Array) {
		ze_target_free(target);
		return NULL;
	}

	if((blocks = cJSON_GetObjectItem(json, "blocks")) == NULL || blocks->type != cJSON_Object) {
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

	sh_new_strdup(target->blocks);
	shdefault(target->blocks, NULL);

	blocks = blocks->child;
	while(blocks != NULL) {
		ze_block_t* b = ze_block_parse(rt, blocks);

		if(b == NULL) {
			ze_target_free(target);
			return NULL;
		}

		shput(target->blocks, blocks->string, b);

		if(b->toplevel) arrput(target->tree, b);

		blocks = blocks->next;
	}

	for(i = 0; i < shlen(target->blocks); i++) {
		cJSON* json = target->blocks[i].value->json;
		cJSON* next = cJSON_GetObjectItem(json, "next");

		if(next == NULL || next->type != cJSON_String) continue;

		target->blocks[i].value->next	    = shget(target->blocks, next->valuestring);
		target->blocks[i].value->next->prev = target->blocks[i].value;
	}

	for(i = 0; i < shlen(target->blocks); i++) {
		cJSON* json   = target->blocks[i].value->json;
		cJSON* parent = cJSON_GetObjectItem(json, "parent");

		if(parent == NULL || parent->type != cJSON_String) continue;

		target->blocks[i].value->parent = shget(target->blocks, parent->valuestring);
		if(target->blocks[i].value->parent->next == target->blocks[i].value) {
			target->blocks[i].value->parent = NULL;
		} else if(target->blocks[i].value->parent->children == NULL) {
			ze_block_t* v = target->blocks[i].value;

			while(v->prev != NULL) v = v->prev;

			target->blocks[i].value->parent->children = v;
		}
	}

	for(i = 0; i < arrlen(target->tree); i++) {
		ze_block_print(target->tree[i]);
		ze_log("");
	}

	return target;
}

void ze_target_free(ze_target_t* target) {
	int i;

	arrfree(target->tree);

	for(i = 0; i < shlen(target->blocks); i++) ze_block_free(target->blocks[i].value);
	shfree(target->blocks);

	for(i = 0; i < arrlen(target->costumes); i++) ze_costume_free(target->costumes[i]);
	arrfree(target->costumes);

	free(target);
}
