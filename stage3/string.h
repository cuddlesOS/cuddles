#ifndef STRING_H
#define STRING_H

#include "def.h"

isize str_cmp(str s1, str s2);
usize str_find(str s, str tokens);
usize str_parse_num(str s, u8 base, u64 *x);
str str_split_walk(str *s, str sep);

#endif
