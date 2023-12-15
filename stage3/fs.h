#ifndef FS_H
#define FS_H

#include "def.h"

typedef struct {
	usize len;
	void *data;
} file;

file fs_read(const char *filename);

#endif
