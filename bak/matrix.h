#ifndef MATRIX_H
#define MATRIX_H

#include <math.h>
#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

typedef float Vec2i[2];

typedef float Vec2f[2];

typedef float Vec3i[3];

typedef float Vec3f[3];

typedef float Vec4i[4];

typedef float Vec4f[4];

#define MAT4X4_IDENTITY_INITIALIZER {\
	1.0, 0.0, 0.0, 0.0,\
	0.0, 1.0, 0.0, 0.0,\
	0.0, 0.0, 1.0, 0.0,\
	0.0, 0.0, 0.0, 1.0\
}

#define MAT4X4_ZERO_INITIALIZER {0.0}

typedef float Mat4x4f[16];

void mat4x4f_mul_vec4f(const Mat4x4f m, const Vec4f v, Vec4f result);

float vec4f_dot_vec4f(const Vec4f v0, const Vec4f v1);

void mat4x4f_mul_mat4x4f(const Mat4x4f m0, const Mat4x4f m1, Mat4x4f result);

#endif
