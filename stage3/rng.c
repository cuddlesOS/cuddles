#include "rng.h"

int rand()
{
	static unsigned long int next = 1;

	next = next * 1103515245 + 12345;
	return (unsigned int) (next/65535) % 32768;
}
