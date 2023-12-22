#ifndef MATH3D_H
#define MATH3D_H

#include "math.h"

static inline void mat_id(float mat[4][4])
{
	for (int i = 0; i < 4; i++)
	for (int j = 0; j < 4; j++)
		mat[i][j] = i == j ? 1.0 : 0.0;
}

static inline void mat_translate(float mat[4][4], float vec[3])
{
	mat_id(mat);
	for (int i = 0; i < 3; i++)
		mat[3][i] = vec[i];
}

static inline void mat_scale(float mat[4][4], float vec[3])
{
	mat_id(mat);
	for (int i = 0; i < 3; i++)
		mat[i][i] = vec[i];
}

static inline void mat_rot_x(float mat[4][4], float a)
{
	mat_id(mat);
	mat[1][1] = cos(a); mat[2][1] = -sin(a);
	mat[1][2] = sin(a); mat[2][2] = cos(a);
}

static inline void mat_rot_y(float mat[4][4], float a)
{
	mat_id(mat);
	mat[0][0] = cos(a); mat[2][0] = sin(a);
	mat[0][2] = -sin(a); mat[2][2] = cos(a);
}

static inline void mat_rot_z(float mat[4][4], float a)
{
	mat_id(mat);
	mat[0][0] = cos(a); mat[1][0] = -sin(a);
	mat[0][1] = sin(a); mat[1][1] = cos(a);
}

static inline void mat_copy(float dst[4][4], float src[4][4])
{
	for (int i = 0; i < 4; i++)
	for (int j = 0; j < 4; j++)
		dst[i][j] = src[i][j];
}

static inline void mat_mul(float a[4][4], float b[4][4])
{
	float out[4][4];
	for (int i = 0; i < 4; i++)
	for (int j = 0; j < 4; j++) {
		out[i][j] = 0;
		for (int k = 0; k < 4; k++)
			out[i][j] += a[k][j] * b[i][k];
	}
	mat_copy(a, out);
}

static inline void vec_copy(float dst[4], float src[4])
{
	for (int i = 0; i < 4; i++)
		dst[i] = src[i];
}

static inline void mat_mul_vec(float mat[4][4], float vec[4])
{
	float out[4];

	for (int j = 0; j < 4; j++) {
		out[j] = 0;
		for (int k = 0; k < 4; k++)
			out[j] += mat[k][j] * vec[k];
	}

	vec_copy(vec, out);
}


static inline void mat_perspective(float mat[4][4], float fov, float aspect, float near, float far)
{
	float a = 1.0 / tan(fov / 2.0);

	mat_scale(mat, (float []) { a/aspect, a, -(far + near) / (far - near) });
	mat[2][3] = -1.0;
	mat[3][2] = -2.0 * far * near / (far - near);
	mat[3][3] = 0.0;
}

static inline float tri_area(float a[2], float b[2], float c[2])
{
	return (b[0] - a[0])*(c[1] - a[1]) - (b[1] - a[1])*(c[0] - a[0]);
}

#endif
