#ifndef MEMORY_H
#define MEMORY_H

#include "def.h"

void *lmemcpy(void *dst, const void *src, usize bytes);
void *rmemcpy(void *dst, const void *src, usize bytes);
int memcmp(const void *s1, const void *s2, usize n);
u8 memsum(const void *ptr, usize size);
void *memset(void *s, int c, usize n);

#endif
