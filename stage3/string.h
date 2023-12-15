#ifndef STRING_H
#define STRING_H

#include "def.h"

u64 parse_num(u8 **str, u8 base, isize size);
usize find_char(const char *str, char chr);
usize strlen(const char *str);
int strcmp(const char *p1, const char *p2);
int strncmp(const char *p1, const char *p2, usize size);

#endif
