#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include "def.h"

typedef struct __attribute__((packed)) {
	u64 rax, rbx, rcx, rdx, rbp, rdi, rsi, r8, r9, r10, r11, r12, r13, r14, r15;
	u64 which, error_code;
	u64 rip, cs, rflags, rsp, ss;
} interrupt_frame;

void interrupts_init();

extern bool in_isr;

#ifdef DEBUG
void interrupts_unsafe(const char *function);
#define ISR_UNSAFE interrupts_unsafe(__FUNCTION__);
#else
#define ISR_UNSAFE
#endif

#endif
