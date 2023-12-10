#ifndef PIC_H
#define PIC_H

#include "def.h"

typedef enum {
	IRQ_PIT = 0,
	IRQ_KEYBOARD,
	IRQ_CASCADE,
	IRQ_COM2,
	IRQ_COM1,
	IRQ_LPT2,
	IRQ_FLOPPY,
	IRQ_LPT1,
	IRQ_CMOS_CLOCK,
	IRQ_MOUSE,
	IRQ_FREE1,
	IRQ_FREE2,
	IRQ_FREE3,
	IRQ_FPU,
	IRQ_ATA_HDD1,
	IRQ_ATA_HDD2,
} irq_lane;

void disable_irqs();
void enable_irqs();
void unmask_irq(u8 lane);
void ack_irq(u8 lane);
void pic_init();

#endif
