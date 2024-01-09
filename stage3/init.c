#include "def.h"
void kmain();

void _start()
{
	// enable SSE. long mode demands it is present
	u64 cr0;
	asm volatile("mov %%cr0, %0 \n":"=r"(cr0));
	asm volatile("mov %0, %%cr0"::"r"((cr0 & ~(1 << 2)) | (1 << 1)));

	u64 cr4;
	asm volatile("mov %%cr4, %0":"=r"(cr4));
	asm volatile("mov %0, %%cr4"::"r"(cr4 | (1 << 9) | (1 << 10)));

	u16 fpu_cw = 0x37a;
	asm volatile("fldcw %0"::"m"(fpu_cw));

	kmain();
}
