#ifndef FS_H
#define FS_H

#include "def.h"

str fs_read(str filename);

typedef struct {
	str name;
	bool is_dir;
	usize size;
	usize children;
} dirent;

typedef struct {
	usize len;
	dirent *data;
} dir;

dir fs_readdir(str path);

#endif
