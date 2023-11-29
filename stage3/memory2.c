#include "memory.h"

int memcmp(const void *s1, const void *s2, usize n)
{
	for (usize i = 0; i < n; i++) {
		unsigned char c1 = ((const unsigned char *) s1)[i];
		unsigned char c2 = ((const unsigned char *) s2)[i];

		if (c1 != c2)
			return (int) c1 - (int) c2;
	}

	return 0;
}

u8 memsum(const void *ptr, usize size)
{
	u8 sum = 0;
	for (usize i = 0; i < size; i++)
		sum += ((const u8 *) ptr)[i];
	return sum;
}
