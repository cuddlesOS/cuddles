#include "clock.h"
#include "pic.h"
#include "font.h"
#include "io.h"
#include "heap.h"
#include "halt.h"

#define SYNC_INTERVAL NANOSECONDS/10

u64 monoclock_rtc_time = 0;
u64 monoclock_last_cycles = 0;

u64 hd_time = 0;
u64 last_cycles = 0;
u64 last_hd_time = 0;
u64 last_sync = 0;
u64 last_ssync = 0;
#define RING 10
u64 ring_pos = 0;
u64 last_cycles_ring[RING] = {0};
u64 last_sync_ring[RING] = {0};
double hd_drift = 0;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Woverflow"
double hd_rate = 1e10000;
#pragma GCC diagnostic pop

#define ADJ_TARGET SYNC_INTERVAL*RING
#define MAX_OVERSHOOT ADJ_TARGET*2

void clock_init()
{
	outb(0x70, 0x8B);
	char prev = inb(0x71);
	outb(0x70, 0x8B);
	outb(0x71, prev | 0x40);
	unmask_irq(8);
}

static inline u64 monoclock(u64 last_cycles, u64 last_hd_time, u64 max_overshoot)
{
	double fcycdiff = clock_cycles() - last_cycles;
	fcycdiff *= hd_drift;
	fcycdiff /= hd_rate;
	if (fcycdiff < 0)
		fcycdiff = 0;
	u64 cycdiff = fcycdiff;
	if (cycdiff > max_overshoot)
		cycdiff = max_overshoot;
	return last_hd_time + cycdiff;
}

void clock_sync()
{
	u64 monotime = monoclock(last_cycles, last_hd_time, MAX_OVERSHOOT);
	u64 cycles = clock_cycles();
	u64 rtc_time = monoclock_rtc_time;
	if (last_sync + SYNC_INTERVAL <= rtc_time) {
		u64 ring_next = (ring_pos + 1) % RING;
		// u64 diff = rtc_time - last_sync_ring[ring_next];
		last_sync = rtc_time;
		last_cycles = cycles;
		last_hd_time = monotime;
		last_sync_ring[ring_pos] = rtc_time;
		last_cycles_ring[ring_pos] = cycles;
		hd_rate = cycles - last_cycles_ring[ring_next];
		hd_drift = (i64)(last_sync) + (i64)(ADJ_TARGET) - (i64)(monotime);
		ring_pos = ring_next;
#ifdef MONOCLOCK_DEBUG
		term_pos old_cursor = font_get_cursor();
		font_set_cursor((term_pos){0,0});
		print(S("RTC: "));
		print_num(monoclock_rtc_time,10);
		print(S("    \nUSR: "));
		print_num(clock_monotonic(),10);
		print(S("    \nADJ: "));
		print_num(monotime,10);
		print(S("    \nTSC: "));
		print_num(cycles,10);
		print(S("    \ndist:  "));
		print_num(diff,10);
		print(S("    \nrate : "));
		print_dbl(hd_rate,4);
		print(S("    \ndrift: "));
		print_dbl(hd_drift,4);
		print(S("    \nadj: "));
		print_dbl(hd_drift/hd_rate,4);
		print(S("    "));
		font_set_cursor(old_cursor);
#endif
	}
}

u64 clock_monotonic_coarse()
{
	return monoclock_rtc_time;
}

u64 clock_monotonic() {
	return monoclock(monoclock_last_cycles, monoclock_rtc_time, RTC_RATE);
}

u64 clock_cycles()
{
	u64 lo,hi;
	asm volatile("rdtsc\n\t": "=a" (lo), "=d" (hi));
	return lo | hi << 32;
}
