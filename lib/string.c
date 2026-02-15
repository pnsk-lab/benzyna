#include <ze_runtime.h>

char* ze_string_dup(const char* str) {
	char* s = malloc(strlen(str) + 1);

	strcpy(s, str);

	return s;
}
