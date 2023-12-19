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
			d = c - 'a';
		else if (c >= 'A' && c <= 'z')
			d = c - 'A';
		else
			return i;

		if (d >= base)
			return i;

		*x = *x * base + d;
	}

    return s.len;
}

str str_split_walk(str *s, str sep)
{
	if (s->len == 0)
		return NILS;

	usize x = str_find(*s, sep);
	usize o = x + (x < s->len);

	s->len -= o;
	s->data += o;

	if (x == 0)
		return str_split_walk(s, sep);
	else
		return (str) { x, s->data - o };
}
