#ifndef FONT_H
#define FONT_H

#include "def.h"

void print(const char *line);
void set_font_size(u16 size);
void print_char(char c);
void print_num(u64 x, u8 base, u8 pad);

#endif
