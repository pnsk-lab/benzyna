#include <ba_runtime.h>

char* ba_string_dup(const char* str) {
	char* s = malloc(strlen(str) + 1);

	strcpy(s, str);

	return s;
}
