#include "cheese3d.h"
#include "math3d.h"
#include "heap.h"
#include "rng.h"
#include "clock.h"

static u32 *make_cheese_texture(u32 tex_w, u32 tex_h, u32 density_min, u32 density_max)
{
	u32 *texture = kmalloc(tex_h * tex_w * sizeof *texture);
	for (u32 y = 0; y < tex_h; y++)
	for (u32 x = 0; x < tex_w; x++) {
		texture[y*tex_w+x] = 0xFFFFDE74;
	}

	int holes = density_min + rand() % (density_max-density_min+1);
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

typedef struct {
	usize num_verts;
	vertex *vertices;
	texture *textures;
	u32 *texture;
	float transform[4][4];
} cheese;

static cheese make_cheese_model(u32 segments, float slice, u32 tex_w, u32 tex_h)
{
	static u32 crust_top = 0xFFFFD35F;
	static u32 crust_back = 0xFFF2B71B;

	usize num_verts = 12*segments+12;
	vertex *vertices = kmalloc(sizeof *vertices * num_verts);
	texture *textures = kmalloc(sizeof *textures * num_verts);

	float angle = 0.0;
	float seg_delta = slice/segments;

	float x = cos(angle);
	float z = sin(angle);

	for (usize i = 0; i < segments; i++) {
		angle += seg_delta;
		float new_x = cos(angle);
		float new_z = sin(angle);

		vertex seg_verts[12] = {
			// top face
			VERT(  0.0, +0.5,   0.0, 0.0, 0.0),
			VERT(    x, +0.5,     z, 0.0, 0.0),
			VERT(new_x, +0.5, new_z, 0.0, 0.0),
			// bottom face (🥺)
			VERT(  0.0, -0.5,   0.0, 0.0, 0.0),
			VERT(new_x, -0.5, new_z, 0.0, 0.0),
			VERT(    x, -0.5,     z, 0.0, 0.0),
			// back face
			QUAD(
				VERT(    x, +0.5,     z, 0.0, 0.0),
				VERT(    x, -0.5,     z, 0.0, 0.0),
				VERT(new_x, -0.5, new_z, 0.0, 0.0),
				VERT(new_x, +0.5, new_z, 0.0, 0.0)
			)
		};

		for (usize j = 0; j < 12; j++) vertices[i*12+j] = seg_verts[j];
		for (usize j = 0; j <  6; j++) textures[i*12+j] = (texture) { 1, 1, &crust_top };
		for (usize j = 6; j < 12; j++) textures[i*12+j] = (texture) { 1, 1, &crust_back };

		x = new_x;
		z = new_z;
	}

	float start_x = cos(0.0);
	float start_z = sin(0.0);

	float end_x = cos(slice);
	float end_z = sin(slice);

	vertex sides[12] = {
		// left face
		QUAD(
			VERT(    0.0, +0.5,     0.0, 0.5, 0.0),
			VERT(    0.0, -0.5,     0.0, 0.5, 1.0),
			VERT(start_x, -0.5, start_z, 0.0, 1.0),
			VERT(start_x, +0.5, start_z, 0.0, 0.0)
		),
		// right face
		QUAD(
			VERT(  end_x, +0.5,   end_z, 1.0, 0.0),
			VERT(  end_x, -0.5,   end_z, 1.0, 1.0),
			VERT(    0.0, -0.5,     0.0, 0.5, 1.0),
			VERT(    0.0, +0.5,     0.0, 0.5, 0.0)
		),
	};

	u32 *tex = make_cheese_texture(tex_w*2, tex_h, 40, 50);
	u32 off = segments*12;
	for (usize i = 0; i < 12; i++) vertices[off+i] = sides[i];
	for (usize i = 0; i < 12; i++) textures[off+i] = (texture) { tex_w*2, tex_h, tex };

	cheese ch = {
		.num_verts = num_verts,
		.vertices = vertices,
		.textures = textures,
		.texture = tex,
		.transform = {{ 0.0 }},
	};

	mat_translate(ch.transform, (float []) { -0.5, 0.0, 0.0 });

	float tmp[4][4];
	mat_rot_y(tmp, slice/2.0);
	mat_mul(ch.transform, tmp);

	return ch;
}

void cheese_demo()
{
	cheese3d_ctx ctx = cheese3d_create_default(0xFF000000);
	cheese ch = make_cheese_model(8, rad(60), 500, 375);

	float angle = 0;
	u64 time = clock_monotonic();

	for (;;) {
		u64 time1 = clock_monotonic();
		float delta = (time1 - time) / (float)(NANOSECONDS);
		time = time1;

		angle += delta * 90;

		float transform[4][4];
		float tmp[4][4];

		mat_perspective(transform, rad(45.0), (float)ctx.width/(float)ctx.height, 0.01, 25.0);

		mat_translate(tmp, (float []) { 0.0, -0.0, 10.0 });
		mat_mul(transform, tmp);

		mat_rot_x(tmp, rad(-20));
		mat_mul(transform, tmp);

		mat_rot_y(tmp, rad(angle));
		mat_mul(transform, tmp);

		mat_scale(tmp, (float []){ 4.0, 3.0, 4.0 });
		mat_mul(transform, tmp);

		mat_mul(transform, ch.transform);

		cheese3d_clear(ctx, true, true);
		cheese3d_render(ctx, ch.num_verts, ch.vertices, ch.textures, transform);
		cheese3d_display(ctx);
	}

	cheese3d_destroy(ctx);

	kfree(ch.textures);
	kfree(ch.vertices);
	kfree(ch.texture);
}
