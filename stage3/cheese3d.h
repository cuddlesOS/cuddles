#ifndef CHEESE3D_H
#define CHEESE3D_H

// Cheese3D is the cuddlesOS 3d graphics API

#include "def.h"

typedef struct {
	float pos[3];
	float tex[2];
} vertex;

typedef struct {
	u32 w, h;
	u32 *data;
} texture;

typedef struct {
	u32 width;
	u32 height;
	u32 pitch;
	u32 bgcolor;
	float *depth_buffer;
	void *color_buffer;
} cheese3d_ctx;

#define VERT(x, y, z, s, t) { { x, y, z }, { s, t } } // this exists to work with the QUAD macro
#define QUAD(a, b, c, d) a, b, c, a, c, d

cheese3d_ctx cheese3d_create(u32 width, u32 height, u32 pitch, u32 bgcolor);
cheese3d_ctx cheese3d_create_default(u32 bgcolor);
void cheese3d_destroy(cheese3d_ctx ctx);
void cheese3d_clear(cheese3d_ctx ctx, bool color, bool depth);
void cheese3d_render(cheese3d_ctx ctx, usize num, vertex *vertices, texture *textures, float transform[4][4]);
void cheese3d_display(cheese3d_ctx ctx);

#endif
