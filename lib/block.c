#include <ba_runtime.h>

ba_block_t* ba_block_parse(ba_runtime_t* rt, ba_cJSON* json) {
	ba_block_t* block = malloc(sizeof(*block));
	cJSON*	    toplevel;
	cJSON*	    opcode;
	cJSON*	    inputs;

	memset(block, 0, sizeof(*block));

	block->json = json;

	if((toplevel = cJSON_GetObjectItem(json, "topLevel")) != NULL && toplevel->type == cJSON_True) {
		block->toplevel = ba_true;
	}

	if((opcode = cJSON_GetObjectItem(json, "opcode")) == NULL || opcode->type != cJSON_String) {
		ba_block_free(block);
		return NULL;
	}

	sh_new_strdup(block->inputs);

	block->opcode = ba_string_dup(opcode->valuestring);

	if((inputs = cJSON_GetObjectItem(json, "inputs")) != NULL && inputs->type == cJSON_Object) {
		inputs = inputs->child;

		while(inputs != NULL) {
			ba_input_t v;
			cJSON*	   type = cJSON_GetArrayItem(inputs, 0);
			cJSON*	   ent	= cJSON_GetArrayItem(inputs, 1);

			if(type != NULL && ent != NULL && type->type == cJSON_Number && ent->type == cJSON_Array) {
				cJSON* enttype = cJSON_GetArrayItem(ent, 0);
				cJSON* entval  = NULL;

				if(enttype != NULL && enttype->type == cJSON_Number) {
					if(4 <= enttype->valuedouble && enttype->valuedouble <= 8) {
						entval	   = cJSON_GetArrayItem(ent, 1);
						v.u.number = (entval->type == cJSON_Number) ? entval->valuedouble : atof(entval->valuestring);
						v.type	   = ba_input_number;
					} else if(enttype->valuedouble == 10) {
						entval	   = cJSON_GetArrayItem(ent, 1);
						v.u.string = ba_string_dup(entval->valuestring);
						v.type	   = ba_input_string;
					} else if(enttype->valuedouble == 11) {
						entval	      = cJSON_GetArrayItem(ent, 2);
						v.u.broadcast = ba_string_dup(entval->valuestring);
						v.type	      = ba_input_broadcast;
					} else if(enttype->valuedouble == 12) {
						entval	     = cJSON_GetArrayItem(ent, 2);
						v.u.variable = ba_string_dup(entval->valuestring);
						v.type	     = ba_input_variable;
					} else if(enttype->valuedouble == 13) {
						entval	 = cJSON_GetArrayItem(ent, 2);
						v.u.list = ba_string_dup(entval->valuestring);
						v.type	 = ba_input_list;
					}
				}

				if(entval != NULL) shput(block->inputs, inputs->string, v);
			} else if(type != NULL && ent != NULL && type->type == cJSON_Number && ent->type == cJSON_String) {
				ba_input_t v;

				v.u.block = ba_string_dup(ent->valuestring);
				v.type	  = ba_input_block;

				shput(block->inputs, inputs->string, v);
			}

			inputs = inputs->next;
		}
	}

	return block;
}

static void _ba_block_print(ba_block_t* block, int indent) {
	char	    n[512];
	int	    i;
	ba_block_t* b;

	for(i = 0; i < indent; i++) n[i] = ' ';
	n[i] = 0;

	ba_log("%s%s", n, block->opcode);
	b = block->next;
	while(b != NULL) {
		ba_log("%s%s", n, b->opcode);

		if(b->children != NULL) _ba_block_print(b->children, indent + 2);

		b = b->next;
	}
}

void ba_block_print(ba_block_t* block) {
	_ba_block_print(block, 0);
}

void ba_block_free(ba_block_t* block) {
	int i;

	for(i = 0; i < shlen(block->inputs); i++) {
		switch(block->inputs[i].value.type) {
		case ba_input_string:
		case ba_input_broadcast:
		case ba_input_variable:
		case ba_input_list:
		case ba_input_block:
			free(block->inputs[i].value.u.string);
			break;
		}
	}
	shfree(block->inputs);

	if(block->opcode != NULL) free(block->opcode);
	free(block);
}

ba_input_t* ba_block_input(ba_block_t* block, const char* input) {
	int ind;

	if((ind = shgeti(block->inputs, input)) == -1) return NULL;

	return &block->inputs[ind].value;
}
