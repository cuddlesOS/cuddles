#include "string.h"

usize find_char(const char *str, char chr)
{
	usize ret = 0;
	while (*str != chr && *str != '\0')
		str++, ret++;
	return ret;
}

u64 parse_num(u8 **str, u8 base, isize size)
{
	u64 x = 0;

	while (size-- != 0) {
		u8 c = **str;

		u64 d;
		if (c >= '0' && c <= '9')
			d = c - '0';
		else if (c >= 'a' && c <= 'z')
			d = c - 'a';
		else if (c >= 'A' && c <= 'z')
			d = c - 'A';
		else
			return x;

		if (d >= base)
			return x;

		(*str)++;
		x = x * base + d;
	}

    return x;
}

usize strlen(const char *str)
{
	return find_char(str, '\0');
}

int strcmp(const char *p1, const char *p2)
{
	while (*p1 == *p2 && *p1 != '\0')
		p1++, p2++;
	return *p1 - *p2;
}

int strncmp(const char *p1, const char *p2, usize size)
{
	for (usize i = 0; i < size; i++)
		if (p1[i] != p2[i])
			return p1[i]-p2[i];
	return 0;
}
