#include "halt.h"
#include "font.h"

void halt()
{
	for (;;)
		asm volatile("hlt");
}

void panic(char *msg)
{
	print(msg);
	halt();
}
