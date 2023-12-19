#ifndef STRING_H
#define STRING_H

#include "def.h"

isize str_cmp(str s1, str s2);
usize str_find(str s, str tokens);
usize str_parse_num(str s, u8 base, u64 *x);
str str_walk(str *s, str sep);
str str_eat(str s, str tokens);
str str_advance(str s, usize x);
bool str_start(str s, str start);

#endif
