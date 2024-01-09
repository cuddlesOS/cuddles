#include "string.h"
#include "memory.h"
#include "heap.h"

isize str_cmp(str s1, str s2)
{
	if (s1.len != s2.len)
		return (isize) s1.len - (isize) s2.len;

	return memcmp(s1.data, s2.data, s1.len);
}

static bool match_char(char c, str tokens)
{
	for (usize t = 0; t < tokens.len; t++)
		if (c == tokens.data[t])
			return true;

	return false;
}

usize str_find(str s, str tokens)
{
	for (usize i = 0; i < s.len; i++)
		if (match_char(s.data[i], tokens))
			return i;

	return s.len;
}

usize str_parse_num(str s, u8 base, u64 *x)
{
	*x = 0;

	for (usize i = 0; i < s.len; i++) {
		u8 c = s.data[i];

		u64 d;
		if (c >= '0' && c <= '9')
			d = c - '0';
		else if (c >= 'a' && c <= 'z')
			d = c - 'a' + 10;
		else if (c >= 'A' && c <= 'Z')
			d = c - 'A' + 10;
		else
			return i;

		if (d >= base)
			return i;

		*x = (*x) * base + d;
	}

    return s.len;
}

str str_walk(str *s, str sep)
{
	if (s->len == 0)
		return NILS;

	usize x = str_find(*s, sep);
	usize o = x + (x < s->len);

	*s = str_advance(*s, o);

	if (x == 0)
		return str_walk(s, sep);
	else
		return (str) { x, s->data - o };
}

str str_eat(str s, str tokens)
{
	while (s.len > 0 && match_char(s.data[0], tokens))
		s = str_advance(s, 1);
	return s;
}

str str_advance(str s, usize x)
{
	s.len -= x;
	s.data += x;
	return s;
}

bool str_start(str s, str start)
{
	if (s.len < start.len)
		return false;
	s.len = start.len;
	return str_cmp(s, start) == 0;
}

str str_intro(char *c)
{
	usize i = 0;
	while (c[i] != '\0')
		i++;
	return (str) { i, c };
}

str str_clone(str s)
{
	str c = { s.len, kmalloc(s.len) };
	lmemcpy(c.data, s.data, s.len);
	return c;
}
