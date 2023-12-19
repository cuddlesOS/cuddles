#include "pic.h"
#include "io.h"

void disable_irqs()
{
	asm volatile("cli");
}

void enable_irqs()
{
	asm volatile("sti");
}

void ack_irq(u8 lane)
{
	if (lane >= 8)
		outb(IO_PIC2_CTRL, 1 << 5);

	outb(IO_PIC1_CTRL, 1 << 5);
}

void wait_irq()
{
	asm volatile("hlt");
}

void unmask_irq(u8 lane)
{
	u8 port = IO_PIC1_DATA;

	if (lane >= 8) {
		port = IO_PIC2_DATA;
		lane -= 8;
	}

	outb(port, inb(port) & ~(1 << lane));
}

void pic_init()
{
	typedef struct {
		bool ic4 : 1;
		bool single : 1;
		bool addr_interval : 1;
		bool level_triggered : 1;
		bool init : 1;
		unsigned int zeros : 3;
	} __attribute__((packed)) pic_icw1;

	u8 icw1 = BITCAST(((pic_icw1) {
		.ic4 = true,
		.single = false,
		.addr_interval = false,
		.level_triggered = false,
		.init = true,
		.zeros = 0,
	}), pic_icw1, u8);

	outb(IO_PIC1_CTRL, icw1);
	outb(IO_PIC2_CTRL, icw1);

	// map IRQs 0-15 to 0x20-0x2F
	// 0x1F is the highest reserved interrupt
	outb(IO_PIC1_DATA, 0x20);
	outb(IO_PIC2_DATA, 0x28);

	// pic2 is connected to IRQ2
	outb(IO_PIC1_DATA, 1 << 2);
	outb(IO_PIC2_DATA, 2);

	typedef struct {
		bool x86 : 1;
		bool auto_eoi : 1;
		bool primary_buffered : 1;
		bool buffered : 1;
		bool fully_nested : 1;
		unsigned int zeros : 3;
	} __attribute__((packed)) pic_icw4;

	u8 icw4 = BITCAST(((pic_icw4) {
		.x86 = true,
		.auto_eoi = false,
		.primary_buffered = false,
		.buffered = false,
		.fully_nested = false,
		.zeros = 0,
	}), pic_icw4, u8);

	outb(IO_PIC1_DATA, icw4);
	outb(IO_PIC2_DATA, icw4);

	// mask all interrupts
	outb(IO_PIC1_DATA, 0xff);
	outb(IO_PIC2_DATA, 0xff);

	disable_irqs();
}
