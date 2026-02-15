#include <ze_runtime.h>

char* ze_exec(ze_runtime_t* rt, ze_input_t* value) {
	char* b = NULL;

	switch(value->type) {
	case ze_input_number:
		b = malloc(64);
		sprintf(b, "%f", value->u.number);
		break;
	case ze_input_color:
		b = malloc(32);
		strcpy(b, value->u.color);
	case ze_input_string:
	case ze_input_broadcast:
		b = malloc(strlen(value->u.string) + 1);
		strcpy(b, value->u.string);
		break;
	case ze_input_variable:
		printf("!\n");
		break;
	}

	return b;
}
