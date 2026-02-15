#include <ba_runtime.h>

char* ba_exec(ba_thread_t* thread, ba_input_t* value) {
	char*	     b = NULL;
	int	     ind;
	ba_sprite_t* spr = ba_runtime_get_stage_sprite(thread->runtime);

	switch(value->type) {
	case ba_input_number:
		b = malloc(64);
		sprintf(b, BA_FORMAT_DOUBLE, value->u.number);
		break;
	case ba_input_color:
		b = malloc(32);
		strcpy(b, value->u.color);
	case ba_input_string:
	case ba_input_broadcast:
		b = malloc(strlen(value->u.string) + 1);
		strcpy(b, value->u.string);
		break;
	case ba_input_variable:
		if((ind = shgeti(thread->sprite->variables, value->u.variable)) != -1) { /* local */
			b = malloc(strlen(thread->sprite->variables[ind].value) + 1);
			strcpy(b, thread->sprite->variables[ind].value);
		} else if((ind = shgeti(spr->variables, value->u.variable)) != -1) { /* global */
			b = malloc(strlen(spr->variables[ind].value) + 1);
			strcpy(b, spr->variables[ind].value);
		}
		break;
	case ba_input_block:
		if((b = ba_shadow_operator(thread, value->u.block)) != NULL) {
		}
		break;
	}

	return b;
}
