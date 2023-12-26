#include "clock.h"
#include "def.h"
#include "halt.h"
#include "font.h"
#include "heap.h"
#include "pic.h"
#include "thread.h"
#include "io.h"
#include "string.h"
#include "debug.h"

extern u64 idt_entries[256]; // isr.asm

void interrupt_handler(interrupt_frame *frame)
{
	if (frame->which < 32) {
		debug_exception(frame);
	} else if (frame->which-32 < 16) {
		u64 irq = frame->which-32;

		if (irq == 8) {
			outb(0x70, 0x0C);
			inb(0x71);
			monoclock_rtc_time += RTC_RATE;
			monoclock_last_cycles = clock_cycles();
		} else {
			if (queue_write.len == queue_write.cap) {
				panic(S("queue exceeded\n"));
				/*
				// TODO: malloc would cause a race condition
				queue_write.cap = queue_write.cap == 0 ? 1 : queue_write.cap * 2;
				queue_write.data = realloc(queue_write.data, queue_write.cap);
				*/
			}

			event *e = &queue_write.data[queue_write.len++];
			e->irq = irq;

			if (e->irq == 1) {
				e->data.scancode = inb(IO_PS2_DATA);
			}
		}

		ack_irq(irq);
	}
}

typedef struct {
	u16 size;
	u64 addr;
} __attribute__((packed)) idt_descriptor;

idt_descriptor idtr;

void interrupts_init()
{
	typedef struct {
	   u16 offset_1;
	   u16 selector;
	   u8 ist;
	   u8 type_attrs;
	   u16 offset_2;
	   u32 offset_3;
	   u32 zero;
	} __attribute__((packed)) interrupt_descriptor;

	interrupt_descriptor *idt = malloc(256 * sizeof *idt);

	for (int i = 0; i < 255; i++) {
		union {
			struct {
				u16 offset_1;
				u16 offset_2;
				u32 offset_3;
			} __attribute__((packed)) off;
			u64 addr;
		} __attribute__((packed)) isr = {
			.addr = idt_entries[i],
		};

		idt[i] = (interrupt_descriptor) {
			.offset_1 = isr.off.offset_1,
			.selector = 0x8,
			.ist = 0,
			.type_attrs = 0x8E,
			.offset_2 = isr.off.offset_2,
			.offset_3 = isr.off.offset_3,
			.zero = 0,
		};
	}

	idtr = (idt_descriptor) {
		.size = 256 * sizeof *idt - 1,
		.addr = (u64) &idt[0],
	};

	asm("lidt %0" : : "m"(idtr));
}
