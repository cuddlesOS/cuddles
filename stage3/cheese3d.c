#include "heap.h"
#include "math3d.h"
#include "cheese3d.h"
#include "gfx.h"
#include "memory.h"

cheese3d_ctx cheese3d_create(void *target, u32 width, u32 height, u32 pitch, u32 bgcolor)
{
	return (cheese3d_ctx) {
		.target = target,
		.width = width,
		.height = height,
		.pitch = pitch,
		.bgcolor = bgcolor,
		.depth_buffer = kmalloc(width * height * sizeof(u32)),
		.color_buffer = kmalloc(pitch * height),
	};
}

cheese3d_ctx cheese3d_create_default(u32 bgcolor)
{
	return cheese3d_create((void *) (u64) gfx_info->framebuffer, gfx_info->width, gfx_info->height, gfx_info->pitch, bgcolor);
}

void cheese3d_destroy(cheese3d_ctx ctx)
{
	kfree(ctx.depth_buffer);
	kfree(ctx.color_buffer);
}

void cheese3d_clear(cheese3d_ctx ctx, bool color, bool depth)
{
	for (u32 x = 0; x < ctx.width; x++)
	for (u32 y = 0; y < ctx.height; y++) {
		if (color) ctx.depth_buffer[y * ctx.width + x] = 0.0;
		if (depth) *(u32 *) (ctx.color_buffer + y * ctx.pitch + x * sizeof(u32)) = ctx.bgcolor;
	}
}

void cheese3d_render(cheese3d_ctx ctx, usize num, vertex *vertices, texture *textures, float transform[4][4])
{
	for (usize i = 0; i < num; i += 3) {
		float verts[3][4];
		float min[2];
		float max[2];

		for (usize j = 0; j < 3; j++) {
			float coord[4];
			for (int k = 0; k < 3; k++)
				coord[k] = vertices[i+j].pos[k];
			coord[3] = 1.0;

			mat_mul_vec(transform, coord);

			verts[j][3] = 1/coord[3];
			for (int k = 0; k < 3; k++)
				verts[j][k] = coord[k] * verts[j][3];

			for (int k = 0; k < 2; k++) {
				if (j == 0 || verts[j][k] > max[k])
					max[k] = verts[j][k];
				if (j == 0 || verts[j][k] < min[k])
					min[k] = verts[j][k];
			}
		}

		// cull clockwise faces
		float area = tri_area(verts[0], verts[1], verts[2]);
		if (area < 0)
			continue;

		i32 i_min[2];
		i32 i_max[2];
		for (int k = 0; k < 2; k++) {
			i32 size = k ? ctx.height : ctx.width;

			i_min[k] = (0.5 + 0.5 * min[k]) * size;
			i_max[k] = (0.5 + 0.5 * max[k]) * size;

			if (i_max[k] >= size)
				i_max[k] = size-1;
			if (i_min[k] < 0)
				i_min[k] = 0;
		}

		for (i32 y = i_min[1]; y <= i_max[1]; y++) {
			float point[2] = {
				0.0,
				(float) y / ctx.height * 2.0 - 1.0
			};
			for (i32 x = i_min[0]; x <= i_max[0]; x++) {
				point[0] = (float) x / ctx.width * 2.0 - 1.0;

				float weights[3];
				if ((weights[0] = tri_area(point, verts[1], verts[2])) < 0)
					continue;
				if ((weights[1] = tri_area(verts[0], point, verts[2])) < 0)
					continue;
				if ((weights[2] = tri_area(verts[0], verts[1], point)) < 0)
					continue;

				float depth = 0.0;
				float w = 0.0;
				float tex[2] = { 0.0, 0.0 };

				for (int j = 0; j < 3; j++) {
					weights[j] /= area;

					depth += weights[j] * verts[j][2];
					w += weights[j] * verts[j][3];
					tex[0] += weights[j] * verts[j][3] * vertices[i+j].tex[0];
					tex[1] += weights[j] * verts[j][3] * vertices[i+j].tex[1];
				}

				tex[0] /= w;
				tex[1] /= w;

				float *d = &ctx.depth_buffer[y * ctx.width + x];
				if (*d > depth)
					continue;
				*d = depth;

				texture *t = &textures[i];

				i32 i_tex[2];
				for (int k = 0; k < 2; k++) {
					i32 size = k ? t->h : t->w;

					i_tex[k] = tex[k] * size;

					if (i_tex[k] >= size)
						i_tex[k] = size-1;
					if (i_tex[k] < 0)
						i_tex[k] = 0;
				}

				*(u32 *) (ctx.color_buffer + y * ctx.pitch + x * sizeof(u32)) =
					t->data[i_tex[1]*t->w+i_tex[0]];
			}
		}

	}

}

void cheese3d_display(cheese3d_ctx ctx)
{
	lmemcpy(ctx.target, ctx.color_buffer, ctx.pitch * ctx.height);
}
