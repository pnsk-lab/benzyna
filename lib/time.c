#include <ba_runtime.h>

#if defined(_WIN32)
double ba_time_tick(void) {
	return (double)GetTickCount() / 1000;
}

void ba_time_sleep(double tick) {
	Sleep(tick / 1000.0);
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

void ba_time_sleep(double tick) {
	struct timespec ts;

	ts.tv_sec  = tick;
	ts.tv_nsec = tick * 1000 * 1000000;

	nanosleep(&ts, NULL);
}
#endif
