#include <ze_runtime.h>

void ze_log(const char* fmt, ...) {
	va_list va;

	fprintf(stderr, "[ZE] ");
	va_start(va, fmt);
	vfprintf(stderr, fmt, va);
	va_end(va);
	fprintf(stderr, "\n");
}
