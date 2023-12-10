#include "font.h"
#include "letters.h"
#include "gfx.h"

// important: must be a multiple of 2, else code won't work
#define TAB_SIZE 4

static u16 font_size;

static u16 outer_width, outer_height;
static u16 cursor_x, cursor_y;

static u16 screen_width, screen_height;

void set_font_size(u16 size)
{
	font_size = size;

	outer_width  = (LETTER_WIDTH  + 2) * font_size;
	outer_height = (LETTER_HEIGHT + 2) * font_size;

	screen_width  = gfx_info->width  / outer_width;
	screen_height = gfx_info->height / outer_height;
}

static void render_char(u8 c)
{
	u16 base_x = cursor_x * outer_width;
	u16 base_y = cursor_y * outer_height;

	gfx_set_area(base_x, base_y, outer_width, outer_height, 0xFF000000);

	for (u16 x = 0; x < LETTER_WIDTH;  x++)
	for (u16 y = 0; y < LETTER_HEIGHT; y++) {
		if (!letters[c].data[y * LETTER_WIDTH + x])
			continue;

		gfx_set_area(
			base_x + (x + 1) * font_size,
			base_y + (y + 1) * font_size,
			font_size, font_size, 0xFFFFFFFF);
	}
}

static void fix_cursor()
{
	while (cursor_x >= screen_width) {
		cursor_x -= screen_width;
		cursor_y++;
	}

	while (cursor_y >= screen_height) {
		gfx_set_area(0, 0, gfx_info->width, gfx_info->height, 0xFF000000);
		cursor_y -= screen_height;
	}
}

void print_char(char c)
{
	switch (c) {
		case '\n':
			cursor_y++;
			cursor_x = 0;
			break;

		case '\t':
			cursor_x = (cursor_x + TAB_SIZE) & ~(TAB_SIZE - 1);
			break;

		case '\b':
			if (cursor_x > 0)
				cursor_x--;
			break;

		case '\r':
			cursor_x = 0;
			break;

		case '\v':
			// vertical tab intentionally unimplemented
			break;

		case '\a':
			// todo: bell
			break;

		case '\f':
			cursor_y = 0;
			cursor_x = 0;
			break;

		default:
			render_char(c);
			cursor_x++;
	}

	fix_cursor();
}

void print(const char *line)
{
	while (*line != '\0')
		print_char(*line++);
}


void print_num(u64 x, u8 base, u8 pad)
{
	char digit[65];
	char *ptr = &digit[64];
	*ptr = '\0';

	do {
		u8 digit = x % base;
		*--ptr = digit + (digit < 10 ? '0' : ('A' - 10));
		x /= base;
	} while (x != 0);

	while (ptr > digit + 64 - pad)
		*--ptr = ' ';

	print(ptr);
}
