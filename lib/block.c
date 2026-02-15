#include <ze_runtime.h>

ze_block_t* ze_block_parse(ze_runtime_t* rt, ze_cJSON* json) {
	ze_block_t* block = malloc(sizeof(*block));
	cJSON*	    toplevel;
	cJSON*	    opcode;

	memset(block, 0, sizeof(*block));

	block->json = json;

	if((toplevel = cJSON_GetObjectItem(json, "topLevel")) != NULL && toplevel->type == cJSON_True) {
		block->toplevel = ze_true;
	}

	if((opcode = cJSON_GetObjectItem(json, "opcode")) == NULL || opcode->type != cJSON_String) {
		ze_block_free(block);
		return NULL;
	}

	block->opcode = ze_string_dup(opcode->valuestring);

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
	if(block->opcode != NULL) free(block->opcode);
	free(block);
}
