#ifndef BOOTINFO_H
#define BOOTINFO_H

#include "def.h"

typedef enum {
	MEM_USABLE = 1,
	MEM_RESERVED = 2,
	MEM_ACPI_RECLAIMABLE = 3,
	MEM_ACPI_NVS = 4,
	MEM_BAD = 5,
} mem_region_type;

typedef struct __attribute__((packed)) {
	void *start;
	usize size;
	u32 type;
	u32 acpi_attrs;
} mem_region;

MKVEC(mem_region)

extern struct __attribute__((packed)) bootinfo {
	u64 ksize;
	u16 gfx_pitch;
	u16 gfx_width;
	u16 gfx_height;
	void *gfx_framebuffer;
	slice_mem_region mmap;
} *bootinfo;

#endif
