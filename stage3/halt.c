#include "halt.h"
#include "font.h"

void halt()
{
	for (;;)
		;
}

void panic(char *msg)
{
	print(msg);
	halt();
}
