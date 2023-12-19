#include "halt.h"
#include "font.h"
#include "pic.h"

void freeze()
{
	for (;;)
		wait_irq();
}

void panic(str msg)
{
	print(msg);
	freeze();
}
