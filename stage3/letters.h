#ifndef _LETTERS_H_
#define _LETTERS_H_

#include "def.h"

#define LETTER_WIDTH  3
#define LETTER_HEIGHT 5

typedef struct __attribute__((packed)) {
	u8 data[LETTER_HEIGHT * LETTER_WIDTH];
} Letter;

extern Letter letters[];

void letters_init();

#endif
