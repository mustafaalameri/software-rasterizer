#include "matrix.h"

void mat4x4f_mul_vec4f(const Mat4x4f m, const Vec4f v, Vec4f result) {
	result[0] = m[0] * v[0] + m[4] * v[1] + m[8] * v[2] + m[12] * v[3];
	result[1] = m[1] * v[0] + m[5] * v[1] + m[9] * v[2] + m[13] * v[3];
	result[2] = m[2] * v[0] + m[6] * v[1] + m[10] * v[2] + m[14] * v[3];
	result[3] = m[3] * v[0] + m[7] * v[1] + m[11] * v[2] + m[15] * v[3];
}

float vec4f_dot_vec4f(const Vec4f v0, const Vec4f v1) {
	return v0[0] * v1[0] + v0[1] * v1[1] + v0[2] * v1[2] + v0[3] * v1[3];
}

void mat4x4f_mul_mat4x4f(const Mat4x4f m0, const Mat4x4f m1, Mat4x4f result) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			result[i * 4 + j] = 0;
 
			for (int k = 0; k < 4; k++) {
				result[i * 4 + j] += m0[k * 4 + i] * m1[j * 4 + k];
			}
		}
	}
}
