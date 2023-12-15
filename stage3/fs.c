#include "fs.h"
#include "ata.h"
#include "string.h"
#include "memory.h"
#include "heap.h"

file fs_read(const char *filename)
{
	u64 start = (*(u32 *) (0x1000-10-8))/512;

	for (;;) {
		u8 *info = ata_read_full(start, 1);

		if (memcmp(info+257, "ustar", 5) != 0) {
			free(info);
			return (file) { .len = 0, .data = nil };
		}

		u8 *infop = info+124;
		usize fsize = parse_num(&infop, 8, 11);
		usize fsect = (fsize+511)/512;

		if (memcmp(info, filename, strlen(filename) + 1) == 0) {
			free(info);
			return (file) { .len = fsize, .data = ata_read_full(start+1, fsect) };
		} else {
			free(info);
			start += 1 + fsect;
		}
	}
}
