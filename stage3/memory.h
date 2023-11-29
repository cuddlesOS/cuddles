#ifndef _MEMORY_H_
#define _MEMORY_H_

#include "def.h"

void memcpy(void *dst, const void *src, usize bytes); // memory.asm
void memmove(void *dst, const void *src, usize bytes); // memory.asm
int memcmp(const void *s1, const void *s2, usize n); // memory2.c
u8 memsum(const void *ptr, usize size); // memory2.c

#endif
