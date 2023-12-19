#include "fs.h"
#include "ata.h"
#include "string.h"
#include "memory.h"
#include "heap.h"

str fs_read(str filename)
{
	u64 start = (*(u32 *) (0x1000-10-8))/512;

	for (;;) {
		u8 *info = ata_read_full(start, 1);

		if (memcmp(info+257, "ustar", 5) != 0) {
			free(info);
			return NILS;
		}

		usize fsize;
		str_parse_num((str) { 11, (char *) info+124 }, 8, &fsize);

		usize fsect = (fsize+511)/512;

		if (memcmp(info, filename.data, filename.len) == 0 && info[filename.len] == '\0') {
			free(info);
			return (str) { .len = fsize, .data = ata_read_full(start+1, fsect) };
		} else {
			free(info);
			start += 1 + fsect;
		}
	}
}
