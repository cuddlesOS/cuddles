#ifndef GFX_H
#define GFX_H

#include "def.h"

extern struct __attribute__((packed)) GfxInfo {
	u16 pitch;
	u16 width;
	u16 height;
	u32 framebuffer;
} *gfx_info;

typedef struct __attribute__((packed)) {
	u8 r, g, b, a;
} color;

u32 make_color(color col);
void gfx_set_pixel(u16 x, u16 y, u32 col);
void gfx_set_area(u16 x, u16 y, u16 w, u16 h, u32 col);
void gfx_draw_img(u16 x, u16 y, u16 w, u16 h, u32 *img);

#endif
