#ifndef FONT_H
#define FONT_H

#include "def.h"

void font_init();
void font_set_size(u16 size);
void font_load_blob(const void *blob);
void font_load_classic();

void print(const char *line);
void printn(const char *line, usize len);
void print_char(char c);
void print_num(u64 x, u8 base, u8 pad_len);
void print_padded(u64 x, u8 base, u8 pad_len, char pad_char);

#endif
