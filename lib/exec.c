#include <ba_runtime.h>

char* ba_exec(ba_runtime_t* rt, ba_input_t* value) {
	char* b = NULL;

	switch(value->type) {
	case ba_input_number:
		b = malloc(64);
		sprintf(b, "%f", value->u.number);
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
		printf("!\n");
		break;
	}

	return b;
}
