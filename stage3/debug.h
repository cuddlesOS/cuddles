#ifndef DEBUG_H
#define DEBUG_H

#include "interrupts.h"

void debug_init();
void debug_exception(interrupt_frame *frame);

#endif
