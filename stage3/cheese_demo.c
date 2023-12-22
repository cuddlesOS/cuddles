#include "cheese3d.h"
#include "math3d.h"
#include "heap.h"
#include "rng.h"

static u32 *make_cheese_texture(u32 tex_w, u32 tex_h)
{
	u32 *texture = malloc(tex_h * tex_w * sizeof *texture);
	for (u32 y = 0; y < tex_h; y++)
	for (u32 x = 0; x < tex_w; x++) {
		texture[y*tex_w+x] = 0xFFFFDE74;
	}

	int holes = 25 + rand() % 5;
	for (int i = 0; i < holes; i++) {
		u32 xo = rand() % tex_w;
		u32 yo = rand() % tex_h;
		i32 radius = 20 + rand() % 5;

		for (i32 xi = -radius; xi <= radius; xi++) {
			i32 x = xi+xo;
			if (!(x >= 0 && x < (i32)tex_w))
				continue;
			double hi = sin(acos((double) xi / (double) radius))*radius;
			for (i32 yi = -hi; yi <= hi; yi++) {
				i32 y = yi+yo;
				if (!(y >= 0 && y < (i32)tex_h))
					continue;
				texture[y*tex_w+x] = 0xFFFCA425;
			}
		}
	}

	return texture;
}

void cheese_demo()
{
	cheese3d_ctx ctx = cheese3d_create_default(0xFF000000);

	const u32 tex_w = 500;
	const u32 tex_h = 375;

	u32 crust_top = 0xFFFFD35F;
	u32 crust_back = 0xFFF2B71B;
	u32 *crust_right = make_cheese_texture(tex_w, tex_h);
	u32 *crust_left = make_cheese_texture(tex_w, tex_h);

	texture textures[24];
	for (int i =  0; i <  6; i++) textures[i] = (texture) { 1, 1, &crust_top };
	for (int i =  6; i < 12; i++) textures[i] = (texture) { 1, 1, &crust_back };
	for (int i = 12; i < 18; i++) textures[i] = (texture) { tex_w, tex_h, crust_right };
	for (int i = 18; i < 24; i++) textures[i] = (texture) { tex_w, tex_h, crust_left };

	// triangles are counter-clockwise
	vertex prism[24] = {
		// top face
		VERT(-0.5, +0.5, -0.5, 0.0, 0.0),
		VERT(+0.5, +0.5, -0.5, 0.0, 0.0),
		VERT( 0.0, +0.5, +0.5, 0.0, 0.0),
		// bottom face (🥺)
		VERT(-0.5, -0.5, -0.5, 0.0, 0.0),
		VERT( 0.0, -0.5, +0.5, 0.0, 0.0),
		VERT(+0.5, -0.5, -0.5, 0.0, 0.0),
		// back face
		QUAD(
			VERT(-0.5, +0.5, -0.5, 0.0, 0.0),
			VERT(-0.5, -0.5, -0.5, 0.0, 0.0),
			VERT(+0.5, -0.5, -0.5, 0.0, 0.0),
			VERT(+0.5, +0.5, -0.5, 0.0, 0.0)
		),
		// right face
		QUAD(
			VERT(+0.5, +0.5, -0.5, 0.0, 0.0),
			VERT(+0.5, -0.5, -0.5, 0.0, 1.0),
			VERT( 0.0, -0.5, +0.5, 1.0, 1.0),
			VERT( 0.0, +0.5, +0.5, 1.0, 0.0)
		),
		// left face
		QUAD(
			VERT( 0.0, +0.5, +0.5, 1.0, 0.0),
			VERT( 0.0, -0.5, +0.5, 1.0, 1.0),
			VERT(-0.5, -0.5, -0.5, 0.0, 1.0),
			VERT(-0.5, +0.5, -0.5, 0.0, 0.0)
		),
	};

	float angle = 0;

	for (;;) {
		angle += 0.3;

		float transform[4][4];
		float tmp[4][4];

		mat_perspective(transform, rad(45.0), (float)ctx.width/(float)ctx.height, 0.01, 25.0);

		mat_translate(tmp, (float []) { 0.0, -0.0, 10.0 });
		mat_mul(transform, tmp);

		mat_rot_x(tmp, rad(-20));
		mat_mul(transform, tmp);

		mat_rot_y(tmp, rad(angle));
		mat_mul(transform, tmp);

		mat_scale(tmp, (float []){ 3.5, 3.0, 4.0 });
		mat_mul(transform, tmp);

		cheese3d_clear(ctx, true, true);
		cheese3d_render(ctx, 24, prism, textures, transform);
		cheese3d_display(ctx);
	}

	cheese3d_destroy(ctx);
	free(crust_right);
	free(crust_left);
}
