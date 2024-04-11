#include "fs.h"
#include "ata.h"
#include "string.h"
#include "memory.h"
#include "heap.h"
#include "bootinfo.h"

#define FS_WALKER(X) bool (X)(str filename, u64 lba, usize fsize, usize fsect, void *varg)

void fs_walk(FS_WALKER(*fun), void *arg)
{
	u64 lba = bootinfo->ksize/512;

	for (;;) {
		u8 *info = ata_read_full(lba, 1);

		if (memcmp(info+257, "ustar", 5) != 0) {
			kfree(info);
			break;
		}

		u64 fsize;
		str_parse_num((str) { 11, (char *) info+124 }, 8, &fsize);

		usize fsect = (fsize+511)/512;
		str filename = str_intro((char *) info);

		bool done = fun(filename, lba, fsize, fsect, arg);

		kfree(info);
		if (done)
			break;

		lba += 1 + fsect;
	}
}

typedef struct {
	str path;
	str result;
} fs_read_arg;

static FS_WALKER(fs_read_walker)
{
	fs_read_arg *arg = varg;

	if (str_cmp(filename, arg->path) != 0)
		return false;

	arg->result = (str) { .len = fsize, .data = ata_read_full(lba+1, fsect) };
	return true;
}

str fs_read(str path)
{
	fs_read_arg arg = {
		.path = path,
		.result = NILS,
	};

	fs_walk(&fs_read_walker, &arg);
	return arg.result;
}

typedef struct {
	str path;
	usize cap;
	dir result;
} fs_readdir_arg;

static str strip_trail(str x)
{
	while (x.len > 0 && x.data[x.len-1] == '/')
		x.len--;
	return x;
}

static FS_WALKER(fs_readdir_walker)
{
	(void) lba, (void) fsect;
	fs_readdir_arg *arg = varg;

	enum {
		HER_SELF,
		HER_CHILD,
		HER_GRANDCHILD,
	} heritage;

	str entn;
	str iter = filename;

	bool match = str_start(iter, arg->path);
	if (!match)
		return false;
	iter = str_advance(iter, arg->path.len);

	if (iter.len == 0) {
		entn = filename;
		heritage = HER_SELF;
	} else {
		if (arg->path.len != 0) {
			if (iter.data[0] != '/')
				return false;
			iter = str_advance(iter, 1);
		}

		if (iter.len == 0) {
			entn = strip_trail(filename);
			heritage = HER_SELF;
		} else {
			entn = str_walk(&iter, S("/"));
			if (iter.len == 0)
				heritage = HER_CHILD;
			else
				heritage = HER_GRANDCHILD;
		}
	}

	bool is_dir = filename.len == 0 || filename.data[filename.len-1] == '/';

	switch (heritage) {
		case HER_SELF:
			if (is_dir)
				return false;

			__attribute__((fallthrough));

		case HER_CHILD:
			if (arg->result.len == arg->cap)
				arg->result.data = krealloc(arg->result.data,
					sizeof(dirent) * (arg->cap = arg->cap ? arg->cap*2 : 1));
			arg->result.data[arg->result.len++] = (dirent) {
				.name = str_clone(entn),
				.size = fsize,
				.is_dir = is_dir,
				.children = 0,
			};

			return heritage == HER_SELF;

		case HER_GRANDCHILD:
			for (usize i = 0; i < arg->result.len; i++) {
				dirent *d = &arg->result.data[i];
				if (str_cmp(d->name, entn) == 0) {
					d->size += fsize;
					d->children++;
					return false;
				}
			}

			// orphan. this shouldn't happen
			return false;
	};

	return false;
}

dir fs_readdir(str path)
{
	fs_readdir_arg arg = {
		.path = strip_trail(path),
		.cap = 0,
		.result = { 0, nil },
	};

	fs_walk(&fs_readdir_walker, &arg);
	return arg.result;
}
