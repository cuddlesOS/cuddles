#include "clock.h"
#include "pic.h"
#include "font.h"
#include "io.h"
#include "thread.h"
#include "heap.h"
#include "halt.h"

u64 monoclock_rtc_time = 0;

void clock_init()
{
	outb(0x70, 0x8B);
	char prev = inb(0x71);
	outb(0x70, 0x8B);
	outb(0x71, prev | 0x40);
	unmask_irq(8);
}

u64 clock_monotonic_coarse() {
	return monoclock_rtc_time;
}

u64 clock_monotonic()
{
	return monoclock_rtc_time; // TODO: high resolution clock
}

u64 clock_cycles()
{
	u64 lo,hi;
	asm volatile("rdtsc\n\t": "=a" (lo), "=d" (hi));
	return lo | hi << 32;
}
