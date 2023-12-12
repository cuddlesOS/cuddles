#ifndef IO_H
#define IO_H

#include "def.h"

static inline void outb(u16 port, u8 val)
{
	asm volatile("outb %0, %1" : : "a"(val), "Nd"(port) : "memory");
}

static inline u8 inb(u16 port)
{
   	u8 val;
	asm volatile("inb %1, %0" : "=a"(val) : "dN"(port) : "memory");
	return val;
}

static inline void outw(u16 port, u16 val)
{
	asm volatile("outw %0, %1" : : "a"(val), "Nd"(port) : "memory");
}

static inline u16 inw(u16 port)
{
   	u16 val;
	asm volatile("inw %1, %0" : "=a"(val) : "dN"(port) : "memory");
	return val;
}

static inline void outl(u16 port, u32 val)
{
	asm volatile("outl %0, %1" : : "a"(val), "Nd"(port) : "memory");
}

static inline u32 inl(u16 port)
{
   	u32 val;
	asm volatile("inl %1, %0" : "=a"(val) : "dN"(port) : "memory");
	return val;
}

static inline void hlt()
{
	asm volatile("hlt");
}

typedef enum {
	IO_PIC1_CTRL = 0x20,
	IO_PIC1_DATA = 0x21,
	IO_PIC2_CTRL = 0xA0,
	IO_PIC2_DATA = 0xA1,
	IO_PS2_DATA = 0x60,
	IO_PS2_CTRL = 0x64,
} io_port;

static inline void io_wait(void)
{
	outb(0x80, 0);
}

#endif
