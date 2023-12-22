#ifndef FONT_H
#define FONT_H

#include "def.h"

typedef struct {
	u16 x,y;
} term_pos;

void font_init();
void font_set_size(u16 size);
void font_set_cursor(term_pos new_cursor);
term_pos font_get_cursor();
term_pos font_get_size();
void font_load_blob(const void *blob);
void font_load_classic();
void font_load_builtin();
void font_clear_screen();

void print(str line);
void print_char(char c);
void print_dec(u64 x);
void print_hex(u64 x);
void print_num(u64 x, u8 base);
void print_num_pad(u64 x, u8 base, u8 pad_len, char pad_char);
void print_dbl(double d, u8 points);

#endif
