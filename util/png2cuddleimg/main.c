#include <png.h>
#include <stdlib.h>
#include <stdio.h>
#include <endian.h>

int main(int argc, char *argv[])
{
#define TRY(expr, ...) if (!(expr)) { \
	fprintf(stderr, "%s: ", argv[0]); \
	fprintf(stderr, __VA_ARGS__); \
	if (file != NULL) fclose(file); \
	png_destroy_read_struct(&png, &info, NULL); \
	return EXIT_FAILURE; }

	FILE *file = NULL;
	png_structp png = NULL;
	png_infop info = NULL;

	TRY(argc == 2, "usage: %s infile.png > outfile.cuddleimg\n", argv[0]);
	TRY((file = fopen(argv[1], "r")) != NULL, "failed to open %s\n", argv[1]);

	TRY(png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL),
		"png_create_read_struct failed\n");

	TRY(info = png_create_info_struct(png), "png_create_info_struct failed\n");

	png_init_io(png, file);
	png_read_info(png, info);

	png_uint_32 width = png_get_image_width(png, info);
	png_uint_32 height = png_get_image_height(png, info);
	png_byte color_type = png_get_color_type(png, info);
	png_byte bit_depth  = png_get_bit_depth(png, info);

	if (bit_depth == 16)
		png_set_strip_16(png);
	if (color_type == PNG_COLOR_TYPE_PALETTE)
		png_set_palette_to_rgb(png);
	if(png_get_valid(png, info, PNG_INFO_tRNS))
		png_set_tRNS_to_alpha(png);
	if (color_type == PNG_COLOR_TYPE_RGB || color_type == PNG_COLOR_TYPE_GRAY_ALPHA || color_type == PNG_COLOR_TYPE_PALETTE)
		png_set_filler(png, 0xFF, PNG_FILLER_AFTER);
	if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
		png_set_gray_to_rgb(png);

	png_read_update_info(png, info);

	fwrite(&width, 1, sizeof width, stdout);
	fwrite(&height, 1, sizeof height, stdout);

	png_uint_32 pitch = png_get_rowbytes(png, info);
	png_byte row[pitch];
	for (png_uint_32 y = 0; y < height; y++) {
		png_read_row(png, row, NULL);

		for (png_uint_32 x = 0; x < width; x++) {
			png_byte tmp = row[4*x];
			row[4*x] = row[4*x+2];
			row[4*x+2] = tmp;
		}

		fwrite(row, 1, pitch, stdout);
	}

	fclose(file);
	png_destroy_read_struct(&png, &info, NULL);
	return EXIT_SUCCESS;
}
