#include <ba_runtime.h>

void ba_log(const char* fmt, ...) {
	va_list va;

	fprintf(stderr, "[BA] ");
	va_start(va, fmt);
	vfprintf(stderr, fmt, va);
	va_end(va);
	fprintf(stderr, "\n");
}
