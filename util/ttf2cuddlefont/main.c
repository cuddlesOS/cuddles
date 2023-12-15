#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#define MAX(a, b) ((a) > (b) ? (a) : (b))

static inline unsigned char bitreverse(unsigned char b) {
	b = (b & 0b11110000) >> 4 | (b & 0b00001111) << 4;
	b = (b & 0b11001100) >> 2 | (b & 0b00110011) << 2;
	b = (b & 0b10101010) >> 1 | (b & 0b01010101) << 1;
	return b;
}

int main(int argc, char *argv[])
{
#define TRY(expr, ...) if (!(expr)) \
	{ fprintf(stderr, "%s: ", argv[0]); fprintf(stderr, __VA_ARGS__); return EXIT_FAILURE; }

	TRY(argc == 2, "usage: %s infile.ttf > outfile.cuddlefont\n", argv[0]);

	FT_Library lib;
	TRY(FT_Init_FreeType(&lib) == 0, "failed to initialize freetype\n");

	FT_Face face;
	TRY(FT_New_Face(lib, argv[1], 0, &face) == 0, "failed to load %s\n", argv[1]);
	// TRY(FT_Set_Pixel_Sizes(face, 0, 16) == 0, "failed to select pixel size of 8x16\n");

	for (int i = 0;; i++) {
		TRY(i != face->num_fixed_sizes, "no  8x16 size available\n");

		if (face->available_sizes[i].height == 16 &&
			face->available_sizes[i].width == 8) {
			TRY(FT_Select_Size(face, i) == 0, "failed to select font size");
			break;
		}
	}

	assert(bitreverse(0b10000000) == 0b00000001);

	for (int i = 0; i < 256; i++) {
		TRY(FT_Load_Glyph(face, FT_Get_Char_Index(face, i), FT_LOAD_DEFAULT) == 0,
			"failed to load glyph %d\n", i);

		TRY(FT_Render_Glyph(face->glyph, FT_RENDER_MODE_MONO) == 0,
			"failed to render glyph %d\n", i);

		for (unsigned int y = 0; y < 16; y++)
			putchar(bitreverse(face->glyph->bitmap.buffer[y * face->glyph->bitmap.pitch]));
	}
}
