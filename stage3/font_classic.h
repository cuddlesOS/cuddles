#ifndef FONT_CLASSIC_H
#define FONT_CLASSIC_H

#include "def.h"

#define CLASSIC_CHAR_WIDTH  3
#define CLASSIC_CHAR_HEIGHT 5

typedef struct __attribute__((packed)) {
	u8 data[CLASSIC_CHAR_WIDTH * CLASSIC_CHAR_HEIGHT];
} classic_char;

classic_char *font_classic();

#endif
