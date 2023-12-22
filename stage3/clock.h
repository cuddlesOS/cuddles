#ifndef CLOCK_H
#define CLOCK_H

#include "def.h"

#define NANOSECONDS 1000000000
#define RTC_FREQ 1024
#define RTC_RATE NANOSECONDS/RTC_FREQ

extern u64 monoclock_rtc_time;
extern u64 monoclock_last_cycles;

void clock_init();
void clock_sync();

u64 clock_cycles();
u64 clock_monotonic_coarse();
u64 clock_monotonic();

#endif
