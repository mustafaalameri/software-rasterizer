#include "clock.h"
#include <assert.h>
#include <profileapi.h>
#ifndef NO_WINMM
#include <timeapi.h>
#endif

LONGLONG clockFrequency;

void init_clock(void) {
	LARGE_INTEGER frequency;
	assert(QueryPerformanceFrequency(&frequency));
	clockFrequency = frequency.QuadPart;
}

#ifndef NO_WINMM
void set_clock_resolution(UINT resolution) {
	assert(timeBeginPeriod(resolution) == TIMERR_NOERROR);
}
#endif

void sleep(DWORD time) {
	Sleep(time);
}

LONGLONG get_ticks(void) {
	LARGE_INTEGER ticks;
	assert(QueryPerformanceCounter(&ticks));
	return ticks.QuadPart;
}

double get_time(void) {
	return (double) get_ticks() / clockFrequency * 1000.0;
}
