#ifndef MATRIX_H
#define MATRIX_H

#include <stdbool.h>
#include <math.h>
#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

float lerpf(float a, float b, float t);

bool almost_equals(float a, float b, float epsilon);

typedef float Vec2i[2];

typedef float Vec2f[2];

#define VEC2_FROM_VEC_INITIALIZER(vector) {vector[0], vector[1]}

typedef float Vec3i[3];

typedef float Vec3f[3];

#define VEC3_FROM_VEC_INITIALIZER(vector) {vector[0], vector[1], vector[2]}

typedef float Vec4i[4];

typedef float Vec4f[4];

#define VEC4_FROM_VEC_INITIALIZER(vector) {vector[0], vector[1], vector[2], vector[3]}

typedef float Mat4x4f[16];

#define MAT4X4F_IDENTITY_INITIALIZER (Mat4x4f) {\
	1.0, 0.0, 0.0, 0.0,\
	0.0, 1.0, 0.0, 0.0,\
	0.0, 0.0, 1.0, 0.0,\
	0.0, 0.0, 0.0, 1.0\
}

#define MAT4X4F_ZERO_INITIALIZER (Mat4x4f) {0.0}

#ifdef __cplusplus
extern "C" {
#endif

void vec4f_perspective_divide(Vec4f vector);

float vec4f_dot_vec4f(const Vec4f v0, const Vec4f v1);

void mat4x4f_mul_vec4f(const Mat4x4f m, const Vec4f v, Vec4f result);

void mat4x4f_mul_mat4x4f(const Mat4x4f m0, const Mat4x4f m1, Mat4x4f result);

void mat4x4f_mul_mat4x4f_mul_mat4x4f(const Mat4x4f m0, const Mat4x4f m1, const Mat4x4f m2, Mat4x4f result);

void mat4x4f_perspective(float FOV, float aspect, float nearPlaneDistance, float farPlaneDistance, Mat4x4f result);

void mat4x4f_camera(Vec3f position, Vec3f right, Vec3f up, Vec3f direction, Mat4x4f result);

void mat4x4f_translation(Vec3f translation, Mat4x4f result);

void mat4x4f_scale(Vec3f scale, Mat4x4f result);

void mat4x4f_rotation_x(float angle, Mat4x4f result);

void mat4x4f_rotation_y(float angle, Mat4x4f result);

void mat4x4f_rotation_z(float angle, Mat4x4f result);

#endif

#ifdef __cplusplus
}
#endif
