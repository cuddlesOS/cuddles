#ifndef _MEMORY_H_
#define _MEMORY_H_

#include "def.h"

void *memcpy(void *dst, const void *src, usize bytes);
int memcpy_r(void *dst, const void *src, usize bytes);
int memcmp(const void *s1, const void *s2, usize n);
u8 memsum(const void *ptr, usize size);
void *memset(void *s, int c, usize n);

#endif
