#ifndef CLOCK_H
#define CLOCK_H

#include <synchapi.h>

extern LONGLONG clockFrequency;

void init_clock(void);

/* In milliseconds */
void set_clock_resolution(UINT resolution);

/* In milliseconds */
void sleep(DWORD time);

LONGLONG get_ticks(void);

/* In milliseconds */
double get_time(void);

#endif
