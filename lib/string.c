#include <ba_runtime.h>

char* ba_string_dup(const char* str) {
	char* s = malloc(strlen(str) + 1);

	strcpy(s, str);

	return s;
}

char* ba_string_concat(const char* str, ...) {
	va_list	    va;
	char*	    r;
	int	    l = 0;
	const char* s;

	l = strlen(str);
	va_start(va, str);
	while((s = va_arg(va, const char*)) != NULL) {
		l += strlen(s);
	}
	va_end(va);

	r = malloc(l + 1);

	strcpy(r, str);
	va_start(va, str);
	while((s = va_arg(va, const char*)) != NULL) {
		strcat(r, s);
	}
	va_end(va);

	return r;
}
