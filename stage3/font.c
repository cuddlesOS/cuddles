#include "font.h"
#include "gfx.h"

#define FONT_SIZE 3

static const u16 outer_width  = (CHAR_WIDTH  + 2) * FONT_SIZE;
static const u16 outer_height = (CHAR_HEIGHT + 2) * FONT_SIZE;

extern u8 letters['z' - 'a' + 1][CHAR_HEIGHT * CHAR_WIDTH];

static u16 line_count = 0;

static void print_chr(u16 at_x, u16 at_y, char c)
{
	u16 base_x = at_x * outer_width;
	u16 base_y = at_y * outer_height;

	gfx_set_area(base_x, base_y, outer_width, outer_height, 0xFF000000);

	if (c > 'z' || c < 'a')
		return;

	for (u16 x = 0; x < CHAR_WIDTH; x++)
	for (u16 y = 0; y < CHAR_HEIGHT; y++) {
		if (!letters[c - 'a'][y * CHAR_WIDTH + x])
			continue;

		gfx_set_area(
			base_x + (x + 1) * FONT_SIZE,
			base_y + (y + 1) * FONT_SIZE,
			FONT_SIZE, FONT_SIZE, 0xFFFFFFFF);
	}
}

void print(char *line)
{
	for (u16 x = 0; *line != '\0'; ++x, ++line)
		print_chr(x, line_count, *line);

	line_count++;
}

