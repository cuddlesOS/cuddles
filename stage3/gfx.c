#include "gfx.h"

struct GfxInfo *gfxinfo = (void *) (0x1000-10);

// byteswap
u32 make_color(color col)
{
	return ((u32) 0)
		& ((u32) col.r << 0)
		& ((u32) col.g << 8)
		& ((u32) col.b << 16)
		& ((u32) col.a << 24);
}

void set_pixel(u16 x, u16 y, u32 col)
{
	*((u32 *) (u64) (gfxinfo->framebuffer + y * gfxinfo->pitch + x * sizeof col)) = col;
}

void set_region(u16 x, u16 y, u16 w, u16 h, u32 col)
{
	void *cbeg = (void *) (u64) (gfxinfo->framebuffer + y * gfxinfo->pitch + x * sizeof col);
	void *cend = cbeg + h * gfxinfo->pitch;

	for (; cbeg < cend; cbeg += gfxinfo->pitch) {
		void *rbeg = cbeg;
		void *rend = rbeg + w * sizeof col;

		for (; rbeg < rend; rbeg += sizeof col)
			*((u32 *) rbeg) = col;
	}
}
