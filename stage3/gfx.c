#include "gfx.h"
#include "memory.h"

struct GfxInfo *gfx_info = (void *) (0x1000-10);

// byteswap
u32 make_color(color col)
{
	return ((u32) 0)
		| ((u32) col.b << 0)
		| ((u32) col.g << 8)
		| ((u32) col.r << 16)
		| ((u32) col.a << 24);
}

void gfx_set_pixel(u16 x, u16 y, u32 col)
{
	u32 *out = (u32 *) (u64) (gfx_info->framebuffer + y * gfx_info->pitch + x * sizeof col);
	*out = col;
	BARRIER_VAR(out);
}

void gfx_set_area(u16 x, u16 y, u16 w, u16 h, u32 col)
{
	void *cbeg = (void *) (u64) (gfx_info->framebuffer + y * gfx_info->pitch + x * sizeof col);
	void *cend = cbeg + h * gfx_info->pitch;

	for (; cbeg < cend; cbeg += gfx_info->pitch) {
		u32 *rbeg = cbeg;
		u32 *rend = rbeg + w;

		for (; rbeg < rend; rbeg++)
			*rbeg = col;
	}

	BARRIER_VAR(cbeg);
}

void gfx_draw_img(u16 x, u16 y, u16 w, u16 h, u32 *img)
{
	void *cbeg = (void *) (u64) (gfx_info->framebuffer + y * gfx_info->pitch + x * sizeof(color));
	for (u16 yi = 0; yi < h; cbeg += gfx_info->pitch, yi++)
		lmemcpy(cbeg, img + yi * w, w * sizeof(color));

	BARRIER_VAR(cbeg);
}
