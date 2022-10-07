#include "font.h"
#include "letters.h"
#include "gfx.h"

#define FONT_SIZE 2

static const u16 outer_width  = (LETTER_WIDTH  + 2) * FONT_SIZE;
static const u16 outer_height = (LETTER_HEIGHT + 2) * FONT_SIZE;

static u16 cursor_x = 0;
static u16 cursor_y = 0;

static void print_chr(u16 at_x, u16 at_y, u8 c)
{
	u16 base_x = at_x * outer_width;
	u16 base_y = at_y * outer_height;

	//gfx_set_area(base_x, base_y, outer_width, outer_height, 0xFF000000);

	if (c > 127)
		return;

	for (u16 x = 0; x < LETTER_WIDTH;  x++)
	for (u16 y = 0; y < LETTER_HEIGHT; y++) {
		if (!letters[c].data[y * LETTER_WIDTH + x])
			continue;

		gfx_set_area(
			base_x + (x + 1) * FONT_SIZE,
			base_y + (y + 1) * FONT_SIZE,
			FONT_SIZE, FONT_SIZE, 0xFFFFFFFF);
	}
}

void println(char *line)
{
	for (; *line != '\0'; ++cursor_x, ++line)
		print_chr(cursor_x, cursor_y, *line);

	cursor_y++;
	cursor_x = 0;
}

