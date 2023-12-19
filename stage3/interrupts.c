#include "def.h"
#include "halt.h"
#include "font.h"
#include "heap.h"
#include "pic.h"
#include "thread.h"
#include "io.h"

extern u64 idt_entries[256]; // isr.asm

typedef struct __attribute__((packed)) {
	u64 rax, rdx, rcx, rsi, rdi, r8, r9, r10, r11;
	u64 which, error_code;
	u64 rip, cs, rflags, rsp, ss;
} interrupt_frame;

static str exception[32] = {
	S("Division Error"),
	S("Debug"),
	S("Non-maskable Interrupt"),
	S("Breakpoint"),
	S("Overflow"),
	S("Bound Range Exceeded"),
	S("Invalid Opcode"),
	S("Device Not Available"),
	S("Double Fault"),
	S("Coprocessor Segment Overrun"),
	S("Invalid TSS"),
	S("Segment Not Present"),
	S("Stack-Segment Fault"),
	S("General Protection Fault"),
	S("Page Fault"),
	NILS,
	S("x87 Floating-Point Exception"),
	S("Alignment Check"),
	S("Machine Check"),
	S("SIMD Floating-Point Exception"),
	S("Virtualization Exception"),
	S("Control Protection Exception"),
	NILS, NILS, NILS, NILS, NILS, NILS,
	S("Hypervisor Injection Exception"),
	S("VMM Communication Exception"),
	S("Security Exception"),
	NILS,
};

static void dump_frame(interrupt_frame *frame)
{
	print(S("rip = ")); print_hex(frame->rip); print_char('\n');
	print(S("cs = ")); print_hex(frame->cs); print_char('\n');
	print(S("rflags = ")); print_hex(frame->rflags); print_char('\n');
	print(S("rsp = ")); print_hex(frame->rsp); print_char('\n');
	print(S("ss = ")); print_hex(frame->ss); print_char('\n');
}

void interrupt_handler(interrupt_frame *frame)
{
	if (frame->which < 32) {
		// TODO: possible race condition due to printing here
		// when exception happens in printing code itself

		if (exception[frame->which].data == nil) {
			print(S("Unknown Exception "));
			print_dec(frame->which);
		} else {
			print(exception[frame->which]);
		}
		print_char('\n');

		if (frame->which == 13) {
			str bits[8] = {
				S("present"),
				S("write"),
				S("user"),
				S("reserved_write"),
				S("instruction_fetch"),
				S("protection_key"),
				S("shadow_stack"),
				S("software_guard_extensions"),
			};

			u8 err = frame->error_code;

			for (int i = 0; i < 8; i++) {
				print(bits[i]); print(S(" = ")); print((err & 1) ? S("true\n") : S("false\n"));
				err >>= 1;
			}
		} else {
			print(S("error_code = ")); print_dec(frame->error_code); print_char('\n');
		}

		dump_frame(frame);
		freeze();
	} else if (frame->which-32 < 16) {
		if (queue_write.len == queue_write.cap) {
			panic(S("queue exceeded\n"));
			/*
			// TODO: malloc would cause a race condition
			queue_write.cap = queue_write.cap == 0 ? 1 : queue_write.cap * 2;
			queue_write.data = realloc(queue_write.data, queue_write.cap);
			*/
		}

		event *e = &queue_write.data[queue_write.len++];
		e->irq = frame->which-32;

		if (e->irq == 1) {
			e->data.scancode = inb(IO_PS2_DATA);
		}

		ack_irq(e->irq);
	} else {
		// print("Spurious Interrupt "); print_num(frame->which, 10, 0); print("\n");
		// dump_frame(frame);
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
