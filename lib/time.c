#include <ba_runtime.h>

#if defined(_WIN32)
double ba_time_tick(void) {
	return (double)GetTickCount() / 1000;
}
#else
double ba_time_tick(void) {
	struct timespec ts;
	double		n = 0;

	clock_gettime(CLOCK_MONOTONIC, &ts);

	n += (double)ts.tv_nsec / 1000 / 1000 / 1000;
	n += ts.tv_sec;

	return n;
}
#endif
