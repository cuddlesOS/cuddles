#include "rng.h"

static unsigned long int next = 1;

int rand()
{
	next = next * 1103515245 + 12345;
	return (unsigned int) (next/65535) % 32768;
}

void srand(int seed)
{
	next = seed;
}
