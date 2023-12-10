#include "def.h"
#include "halt.h"
#include "font.h"
#include "heap.h"
#include "pic.h"

extern u64 idt_entries[256]; // isr.asm

typedef struct {
	u64 which;
	u64 error_code;
	// automatically pushed
	u64 rip;
	u64 cs;
	u64 rflags;
	u64 rsp;
	u64 ss;
} interrupt_frame;

static const char *exception[32] = {
	"Division Error",
	"Debug",
	"Non-maskable Interrupt",
	"Breakpoint",
	"Overflow",
	"Bound Range Exceeded",
	"Invalid Opcode",
	"Device Not Available",
	"Double Fault",
	"Coprocessor Segment Overrun",
	"Invalid TSS",
	"Segment Not Present",
	"Stack-Segment Fault",
	"General Protection Fault",
	"Page Fault",
	nil,
	"x87 Floating-Point Exception",
	"Alignment Check",
	"Machine Check",
	"SIMD Floating-Point Exception",
	"Virtualization Exception",
	"Control Protection Exception",
	nil, nil, nil, nil, nil, nil,
	"Hypervisor Injection Exception",
	"VMM Communication Exception",
	"Security Exception",
	nil,
};

static void dump_frame(interrupt_frame *frame)
{
	print("rip = "); print_num(frame->rip, 16, 0); print("\n");
	print("cs = "); print_num(frame->cs, 16, 0); print("\n");
	print("rflags = "); print_num(frame->rflags, 16, 0); print("\n");
	print("rsp = "); print_num(frame->rsp, 16, 0); print("\n");
	print("ss = "); print_num(frame->ss, 16, 0); print("\n");
}

void interrupt_handler(interrupt_frame *frame)
{
	if (frame->which < 32) {
		if (exception[frame->which] == nil) {
			print("Unknown Exception ");
			print_num(frame->which, 10, 0);
		} else {
			print(exception[frame->which]);
		}
		print("\n");

		if (frame->which == 13) {
			const char *bits[8] = {
				"present",
				"write",
				"user",
				"reserved_write",
				"instruction_fetch",
				"protection_key",
				"shadow_stack",
				"software_guard_extensions",
			};

			u8 err = frame->error_code;

			for (int i = 0; i < 8; i++) {
				print(bits[i]); print(" = "); print((err & 1) ? "true\n" : "false\n");
				err >>= 1;
			}
		} else {
			print("error_code = "); print_num(frame->error_code, 10, 0); print("\n");
		}

		dump_frame(frame);

		halt();
	} else if (frame->which-32 < 16) {
		u64 irq = frame->which-32;
		print("IRQ "); print_num(irq, 10, 0); print("\n");
		ack_irq(irq);
	} else {
		print("Spurious Interrupt "); print_num(frame->which, 10, 0); print("\n");
		dump_frame(frame);
	}
}

void init_interrupts()
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

	typedef struct {
		u16 size;
		u64 addr;
	} __attribute__((packed)) idt_descriptor;

	idt_descriptor idtr = {
		.size = 256 * sizeof *idt - 1,
		.addr = (u64) &idt[0],
	};

	asm("lidt %0" : : "m"(idtr));
}
