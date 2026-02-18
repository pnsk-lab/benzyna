#include <ba_runtime.h>

ba_target_t* ba_target_parse(ba_runtime_t* rt, ba_cJSON* json) {
	ba_target_t* target = malloc(sizeof(*target));
	cJSON*	     variables;
	cJSON*	     lists;
	cJSON*	     costumes;
	cJSON*	     blocks;
	cJSON*	     sounds;
	cJSON*	     isStage;
	int	     i;

	memset(target, 0, sizeof(*target));

	target->json = json;

	if((variables = cJSON_GetObjectItem(json, "variables")) == NULL || variables->type != cJSON_Object) {
		ba_target_free(target);
		return NULL;
	}

	if((lists = cJSON_GetObjectItem(json, "lists")) == NULL || lists->type != cJSON_Object) {
		ba_target_free(target);
		return NULL;
	}

	if((costumes = cJSON_GetObjectItem(json, "costumes")) == NULL || costumes->type != cJSON_Array) {
		ba_target_free(target);
		return NULL;
	}

	if((sounds = cJSON_GetObjectItem(json, "sounds")) == NULL || sounds->type != cJSON_Array) {
		ba_target_free(target);
		return NULL;
	}

	if((blocks = cJSON_GetObjectItem(json, "blocks")) == NULL || blocks->type != cJSON_Object) {
		ba_target_free(target);
		return NULL;
	}

	if((isStage = cJSON_GetObjectItem(json, "isStage")) != NULL && isStage->type == cJSON_True) {
		target->stage = ba_true;
	}

	sh_new_strdup(target->variables);
	shdefault(target->variables, NULL);

	variables = variables->child;
	while(variables != NULL) {
		if(variables->type == cJSON_Array && cJSON_GetArraySize(variables) == 2) {
			cJSON* e = cJSON_GetArrayItem(variables, 1);

			if(e->type == cJSON_Number) {
				char* s = malloc(64);
				sprintf(s, BA_FORMAT_DOUBLE, e->valuedouble);

				shput(target->variables, variables->string, s);
			} else if(e->type == cJSON_String) {
				char* s = malloc(strlen(e->valuestring) + 1);
				strcpy(s, e->valuestring);

				shput(target->variables, variables->string, s);
			}
		}

		variables = variables->next;
	}

	sh_new_strdup(target->lists);
	shdefault(target->lists, NULL);

	lists = lists->child;
	while(lists != NULL) {
		if(lists->type == cJSON_Array && cJSON_GetArraySize(lists) == 2) {
			cJSON* e = cJSON_GetArrayItem(lists, 1);

			if(e->type == cJSON_Array) {
				char** a = NULL;

				for(i = 0; i < cJSON_GetArraySize(e); i++) {
					cJSON* e2 = cJSON_GetArrayItem(e, i);

					if(e2->type == cJSON_Number) {
						char* s = malloc(64);
						sprintf(s, BA_FORMAT_DOUBLE, e2->valuedouble);

						arrput(a, s);
					} else if(e2->type == cJSON_String) {
						char* s = malloc(strlen(e2->valuestring) + 1);
						strcpy(s, e2->valuestring);

						arrput(a, s);
					}
				}

				shput(target->lists, lists->string, a);
			}
		}

		lists = lists->next;
	}

	costumes = costumes->child;
	while(costumes != NULL) {
		ba_costume_t* c = ba_costume_parse(rt, costumes);
		if(c == NULL) {
			ba_target_free(target);
			return NULL;
		}

		arrput(target->costumes, c);

		costumes = costumes->next;
	}

	sounds = sounds->child;
	while(sounds != NULL) {
		if(sounds->type == cJSON_Object) {
			cJSON* name   = cJSON_GetObjectItem(sounds, "name");
			cJSON* md5ext = cJSON_GetObjectItem(sounds, "md5ext");

			if(name != NULL && md5ext != NULL && name->type == cJSON_String && md5ext->type == cJSON_String) {
				shput(target->sounds, name->valuestring, md5ext->valuestring);
			}
		}

		sounds = sounds->next;
	}

	sh_new_strdup(target->blocks);
	shdefault(target->blocks, NULL);

	blocks = blocks->child;
	while(blocks != NULL) {
		ba_block_t* b = ba_block_parse(rt, blocks);

		if(b == NULL) {
			ba_target_free(target);
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
			ba_block_t* v = target->blocks[i].value;

			while(v->prev != NULL) v = v->prev;

			target->blocks[i].value->parent->children = v;
		}
	}

	for(i = 0; i < arrlen(target->tree); i++) {
		ba_block_print(target->tree[i]);
		ba_log("");
	}

	return target;
}

void ba_target_free(ba_target_t* target) {
	int i;

	arrfree(target->tree);

	for(i = 0; i < shlen(target->sounds); i++) free(target->sounds[i].value);
	shfree(target->sounds);

	for(i = 0; i < shlen(target->variables); i++) free(target->variables[i].value);
	shfree(target->variables);

	for(i = 0; i < shlen(target->lists); i++) {
		int j;

		for(j = 0; j < arrlen(target->lists[i].value); j++) {
			free(target->lists[i].value[j]);
		}
		arrfree(target->lists[i].value);
	}
	shfree(target->variables);

	for(i = 0; i < shlen(target->blocks); i++) ba_block_free(target->blocks[i].value);
	shfree(target->blocks);

	for(i = 0; i < arrlen(target->costumes); i++) ba_costume_free(target->costumes[i]);
	arrfree(target->costumes);

	free(target);
}
