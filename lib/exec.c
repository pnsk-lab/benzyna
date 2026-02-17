#include <ba_runtime.h>

char* ba_exec(ba_thread_t* thread, ba_input_t* value) {
	char*			   b = NULL;
	int			   ind;
	ba_sprite_t*		   spr = ba_runtime_get_stage_sprite(thread->runtime);
	ba_thread_shadow_handler_t handler;
	ba_block_t*		   block;
	char**			   list;

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
	case ba_input_list:
		list = NULL;
		if((ind = shgeti(thread->sprite->target->lists, value->u.list)) != -1) { /* local */
			list = thread->sprite->target->lists[ind].value;
		} else if((ind = shgeti(spr->target->lists, value->u.list)) != -1) { /* global */
			list = spr->target->lists[ind].value;
		}

		if(list != NULL) {
			int i, l = 0;

			for(i = 0; i < arrlen(list); i++) {
				if(i > 0) l++;
				l += strlen(list[i]);
			}

			b    = malloc(l + 1);
			b[0] = 0;

			for(i = 0; i < arrlen(list); i++) {
				if(i > 0) strcat(b, " ");
				strcat(b, list[i]);
			}
		} else {
			b    = malloc(1);
			b[0] = 0;
		}
		break;
	case ba_input_block:
		if((block = shget(thread->sprite->target->blocks, value->u.block)) != NULL && (handler = shget(thread->runtime->shadow_handlers, block->opcode)) != NULL) {
			b = handler(thread, block);
		}
		break;
	}

	return b;
}
