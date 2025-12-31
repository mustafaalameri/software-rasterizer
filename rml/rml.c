#include "rml.h"

#include <string.h>

float lerpf(float a, float b, float t) {
	return a + (b - a) * t;
}

bool almost_equals(float a, float b, float epsilon) {
	return abs(a - b) < epsilon;
}

void vec4f_perspective_divide(Vec4f vector) {
	vector[0] /= vector[3];
	vector[1] /= vector[3];
	vector[2] /= vector[3];
	vector[3] = 1.0; /* Optimisation: w = w/w = 1.0 -> w = 1.0 */
}

float vec4f_dot_vec4f(const Vec4f v0, const Vec4f v1) {
	return v0[0] * v1[0] + v0[1] * v1[1] + v0[2] * v1[2] + v0[3] * v1[3];
}
void mat4x4f_mul_vec4f(const Mat4x4f m, const Vec4f v, Vec4f result) {
	result[0] = m[0] * v[0] + m[4] * v[1] + m[8] * v[2] + m[12] * v[3];
	result[1] = m[1] * v[0] + m[5] * v[1] + m[9] * v[2] + m[13] * v[3];
	result[2] = m[2] * v[0] + m[6] * v[1] + m[10] * v[2] + m[14] * v[3];
	result[3] = m[3] * v[0] + m[7] * v[1] + m[11] * v[2] + m[15] * v[3];
}

void mat4x4f_mul_mat4x4f(const Mat4x4f m0, const Mat4x4f m1, Mat4x4f result) {
	result[0] = m0[0] * m1[0] + m0[4] * m1[1] + m0[8] * m1[2] + m0[12] * m1[3];
	result[1] = m0[1] * m1[0] + m0[5] * m1[1] + m0[9] * m1[2] + m0[13] * m1[3];
	result[2] = m0[2] * m1[0] + m0[6] * m1[1] + m0[10] * m1[2] + m0[14] * m1[3];
	result[3] = m0[3] * m1[0] + m0[7] * m1[1] + m0[11] * m1[2] + m0[15] * m1[3];
	result[4] = m0[0] * m1[4] + m0[4] * m1[5] + m0[8] * m1[6] + m0[12] * m1[7];
	result[5] = m0[1] * m1[4] + m0[5] * m1[5] + m0[9] * m1[6] + m0[13] * m1[7];
	result[6] = m0[2] * m1[4] + m0[6] * m1[5] + m0[10] * m1[6] + m0[14] * m1[7];
	result[7] = m0[3] * m1[4] + m0[7] * m1[5] + m0[11] * m1[6] + m0[15] * m1[7];
	result[8] = m0[0] * m1[8] + m0[4] * m1[9] + m0[8] * m1[10] + m0[12] * m1[11];
	result[9] = m0[1] * m1[8] + m0[5] * m1[9] + m0[9] * m1[10] + m0[13] * m1[11];
	result[10] = m0[2] * m1[8] + m0[6] * m1[9] + m0[10] * m1[10] + m0[14] * m1[11];
	result[11] = m0[3] * m1[8] + m0[7] * m1[9] + m0[11] * m1[10] + m0[15] * m1[11];
	result[12] = m0[0] * m1[12] + m0[4] * m1[13] + m0[8] * m1[14] + m0[12] * m1[15];
	result[13] = m0[1] * m1[12] + m0[5] * m1[13] + m0[9] * m1[14] + m0[13] * m1[15];
	result[14] = m0[2] * m1[12] + m0[6] * m1[13] + m0[10] * m1[14] + m0[14] * m1[15];
	result[15] = m0[3] * m1[12] + m0[7] * m1[13] + m0[11] * m1[14] + m0[15] * m1[15];
}

void mat4x4f_mul_mat4x4f_mul_mat4x4f(const Mat4x4f m0, const Mat4x4f m1, const Mat4x4f m2, Mat4x4f result) {
	Mat4x4f m0Mulm1;
	mat4x4f_mul_mat4x4f(m0, m1, m0Mulm1);
	mat4x4f_mul_mat4x4f(m0Mulm1, m2, result);
}

void mat4x4f_perspective(float FOV, float aspect, float nearPlaneDistance, float farPlaneDistance, Mat4x4f result) {
	float tanHalfFOV = tan(FOV * 0.5);
	float nearPlaneFarPlaneDistance = farPlaneDistance - nearPlaneDistance;
	Mat4x4f matrix = {
		1.0 / (aspect * tanHalfFOV), 0.0, 0.0, 0.0,
		0.0, 1.0 / tanHalfFOV, 0.0, 0.0,
		0.0, 0.0, -(farPlaneDistance + nearPlaneDistance) / nearPlaneFarPlaneDistance, -1.0,
		0.0, 0.0, -2.0 * farPlaneDistance * nearPlaneDistance / nearPlaneFarPlaneDistance, 0.0
	};
	memcpy(result, matrix, sizeof(Mat4x4f));
}

void mat4x4f_camera(Vec3f position, Vec3f right, Vec3f up, Vec3f direction, Mat4x4f result) {
	mat4x4f_mul_mat4x4f((Mat4x4f) {
		right[0], right[1], right[2], 0.0,
		up[0], up[1], up[2], 0.0,
		direction[0], direction[1], direction[2], 0.0,
		0.0, 0.0, 0.0, 1.0
	}, (Mat4x4f) {
		1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		-position[0], -position[1], -position[2], 1.0
	}, result);
}

void mat4x4f_translation(Vec3f translation, Mat4x4f result) {
	Mat4x4f matrix = {
		1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		translation[0], translation[1], translation[2], 1.0
	};
	memcpy(result, matrix, sizeof(matrix));
}

void mat4x4f_scale(Vec3f scale, Mat4x4f result) {
	Mat4x4f matrix = {
		scale[0], 0.0, 0.0, 0.0,
		0.0, scale[1], 0.0, 0.0,
		0.0, 0.0, scale[2], 0.0,
		0.0, 0.0, 0.0, 1.0
	};
	memcpy(result, matrix, sizeof(matrix));
}

void mat4x4f_rotation_x(float angle, Mat4x4f result) {
	Mat4x4f matrix = {
		1.0, 0.0, 0.0, 0.0,
		0.0, cos(angle), -sin(angle), 0.0,
		0.0, sin(angle), cos(angle), 0.0,
		0.0, 0.0, 0.0, 1.0
	};
	memcpy(result, matrix, sizeof(matrix));
}

void mat4x4f_rotation_y(float angle, Mat4x4f result) {
	Mat4x4f matrix = {
		cos(angle), 0.0, sin(angle), 0.0,
		0.0, 1.0, 0.0, 0.0,
		-sin(angle), 0.0, cos(angle), 0.0,
		0.0, 0.0, 0.0, 1.0
	};
	memcpy(result, matrix, sizeof(matrix));
}

void mat4x4f_rotation_z(float angle, Mat4x4f result) {
	Mat4x4f matrix = {
		cos(angle), -sin(angle), 0.0, 0.0,
		sin(angle), cos(angle), 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0
	};
	memcpy(result, matrix, sizeof(matrix));
}
