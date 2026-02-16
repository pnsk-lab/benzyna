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

ba_bool ba_string_is_number(const char* str) {
	char*  end;
	double d = strtod(str, &end);

	if(end == str || end[0] != 0 || errno) return ba_false;

	return ba_true;
}

ba_bool ba_string_is_false(const char* str) {
	if(ba_string_is_number(str) && atof(str) == 0) return ba_false;
	if(strcmp(str, "false") == 0) return ba_false;

	return ba_true;
}
