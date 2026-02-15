#include <ze_runtime.h>

ze_block_t* ze_block_parse(ze_runtime_t* rt, ze_cJSON* json) {
	ze_block_t* block = malloc(sizeof(*block));
	cJSON*	    toplevel;
	cJSON*	    opcode;
	cJSON*	    inputs;

	memset(block, 0, sizeof(*block));

	block->json = json;

	if((toplevel = cJSON_GetObjectItem(json, "topLevel")) != NULL && toplevel->type == cJSON_True) {
		block->toplevel = ze_true;
	}

	if((opcode = cJSON_GetObjectItem(json, "opcode")) == NULL || opcode->type != cJSON_String) {
		ze_block_free(block);
		return NULL;
	}

	sh_new_strdup(block->inputs);

	block->opcode = ze_string_dup(opcode->valuestring);

	if((inputs = cJSON_GetObjectItem(json, "inputs")) != NULL && inputs->type == cJSON_Object) {
		inputs = inputs->child;

		while(inputs != NULL) {
			ze_input_t v;
			int	   vnum;
			cJSON*	   type = cJSON_GetArrayItem(inputs, 0);
			cJSON*	   ent	= cJSON_GetArrayItem(inputs, 1);

			if(type != NULL && ent != NULL && type->type == cJSON_Number && ent->type == cJSON_Array) {
				cJSON* enttype = cJSON_GetArrayItem(ent, 0);
				cJSON* entval  = NULL;

				if(enttype != NULL && enttype->type == cJSON_Number) {
					if(4 <= enttype->valuedouble && enttype->valuedouble <= 8) {
						entval	 = cJSON_GetArrayItem(ent, 1);
						v.number = atof(entval->valuestring);
						vnum	 = ze_input_number;
					} else if(enttype->valuedouble == 10) {
						entval	 = cJSON_GetArrayItem(ent, 1);
						v.string = ze_string_dup(entval->valuestring);
						vnum	 = ze_input_string;
					} else if(enttype->valuedouble == 11) {
						entval	    = cJSON_GetArrayItem(ent, 2);
						v.broadcast = ze_string_dup(entval->valuestring);
						vnum	    = ze_input_broadcast;
					} else if(enttype->valuedouble == 12) {
						entval	   = cJSON_GetArrayItem(ent, 2);
						v.variable = ze_string_dup(entval->valuestring);
						vnum	   = ze_input_variable;
					} else if(enttype->valuedouble == 13) {
						entval = cJSON_GetArrayItem(ent, 2);
						v.list = ze_string_dup(entval->valuestring);
						vnum   = ze_input_list;
					}
				}

				if(entval != NULL) {
					int ind;

					shput(block->inputs, inputs->string, v);
					ind = shgeti(block->inputs, inputs->string);

					block->inputs[ind].type = vnum;
				}
			} else if(type != NULL && ent != NULL && type->type == cJSON_Number) {
				printf("%s: %s\n", inputs->string, cJSON_Print(inputs));
			}

			inputs = inputs->next;
		}
	}

	return block;
}

static void _ze_block_print(ze_block_t* block, int indent) {
	char	    n[512];
	int	    i;
	ze_block_t* b;

	for(i = 0; i < indent; i++) n[i] = ' ';
	n[i] = 0;

	ze_log("%s%s", n, block->opcode);
	b = block->next;
	while(b != NULL) {
		ze_log("%s%s", n, b->opcode);

		if(b->children != NULL) _ze_block_print(b->children, indent + 2);

		b = b->next;
	}
}

void ze_block_print(ze_block_t* block) {
	_ze_block_print(block, 0);
}

void ze_block_free(ze_block_t* block) {
	int i;

	for(i = 0; i < shlen(block->inputs); i++) {
		switch(block->inputs[i].type) {
		case ze_input_string:
		case ze_input_broadcast:
		case ze_input_variable:
		case ze_input_list:
			free(block->inputs[i].value.string);
			break;
		}
	}
	shfree(block->inputs);

	if(block->opcode != NULL) free(block->opcode);
	free(block);
}
