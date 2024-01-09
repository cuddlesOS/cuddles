#include "halt.h"
#include "font.h"
#include "pic.h"

[[noreturn]] void freeze()
{
	for (;;)
		wait_irq();
}

[[noreturn]] void panic(str msg)
{
	print(msg);
	freeze();
}
