/* Some old code */

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "rml.h"

#include <cstdlib>
#include <cstdio>
#include <stdint.h>

#define DEG_TO_RAD(angle) angle * M_PI / 180

void dump_matrix(glm::mat4 matrix) {
	const float *m = (const float *) glm::value_ptr(matrix);

	printf("%f %f %f %f\n%f %f %f %f\n%f %f %f %f\n%f %f %f %f\n", m[0], m[1], m[2], m[3], m[4], m[5], m[6], m[7], m[8], m[9], m[10], m[11], m[12], m[13], m[14], m[15]);
}

void dump_matrix(Mat4x4f m) {
	printf("%f %f %f %f\n%f %f %f %f\n%f %f %f %f\n%f %f %f %f\n", m[0], m[1], m[2], m[3], m[4], m[5], m[6], m[7], m[8], m[9], m[10], m[11], m[12], m[13], m[14], m[15]);
}

void dump_vector(const Vec4f m) {
	printf("%f %f %f %f\n", m[0], m[1], m[2], m[3]);
}

bool almostEquals(float a, float b, float epsilon = FLT_EPSILON) {
	return std::abs(a - b) < epsilon;
}

int currentTestUnit = 0;
int failedTestUnits = 0;

void compare_matrices(Mat4x4f matrixRML, glm::mat4 matrixGLM) {
	const float *rawMatrixGLM = (const float *) glm::value_ptr(matrixGLM);

	for (int i = 0; i < 16; ++i) {
		if (!almostEquals(matrixRML[i], rawMatrixGLM[i])) {
			printf("Test unit %d failed at %d.\nGLM:\n", currentTestUnit, i);
			dump_matrix(matrixGLM);
			puts("RML:");
			dump_matrix(matrixRML);
			currentTestUnit++;
			failedTestUnits++;
			return;
		}
	}
	currentTestUnit++;
}

void compare_vectors(Vec4f matrixRML, glm::vec4 matrixGLM) {
	const float *rawMatrixGLM = (const float *) glm::value_ptr(matrixGLM);

	for (int i = 0; i < 4; ++i) {
		if (!almostEquals(matrixRML[i], rawMatrixGLM[i])) {
			printf("Test unit %d failed at %d.\nGLM:\n", currentTestUnit + 1, i);
			dump_vector(rawMatrixGLM);
			puts("RML:");
			dump_vector(matrixRML);
			currentTestUnit++;
			failedTestUnits++;
			return;
		}
	}
	currentTestUnit++;
}

int main() {
	// 1
	float scale = 0.3;
	glm::mat4 scaleGLM = glm::scale(glm::mat4(1.0), glm::vec3(scale));
	Mat4x4f scaleRML = {
		scale, 0.0, 0.0, 0.0,
		0.0, scale, 0.0, 0.0,
		0.0, 0.0, scale, 0.0,
		0.0, 0.0, 0.0, 1.0
	};
	compare_matrices(scaleRML, scaleGLM);

	// 2
	float rotateZ = M_PI * 0.5;
	glm::mat4 rotateGLM = glm::rotate(glm::mat4(1.0), rotateZ, glm::vec3(0.0, 0.0, -1.0));
	Mat4x4f rotateRML = {
		cos(rotateZ), -sin(rotateZ), 0.0, 0.0,
		sin(rotateZ), cos(rotateZ), 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0
	};
	compare_matrices(rotateRML, rotateGLM);

	// 3
	float translateX = 0.5;
	float translateY = 0.1;
	float translateZ = -0.7;
	glm::mat4 translateGLM = glm::translate(glm::mat4(1.0), glm::vec3(translateX, translateY, translateZ));
	Mat4x4f translateRML = {
		1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		translateX, translateY, translateZ, 1.0
	};
	compare_matrices(translateRML, translateGLM);

	// 4
	float mul0[16] = {
		0.7, 0.0, 3.0, 0.7,
		0.17, 0.88, 0.0, 9.8,
		4.11, 3.0, 0.98, 0.111,
		0.3333, 0.4, 99.0, 100.0
	};
	float mul1[16] = {
		9.0, 8.0, 77.4, 0.3,
		0.333, 0.98, 8.9, 3.31,
		0.5, 4.56, 7.1, 1.2,
		1.23, 4.32, 6.7, 0.0
	};
	glm::mat4 mulGLM = glm::make_mat4(mul0) * glm::make_mat4(mul1);
	Mat4x4f mulRML;
	mat4x4f_mul_mat4x4f(mul0, mul1, mulRML);
	compare_matrices(mulRML, mulGLM);

	// 5
	glm::mat4 modelGLM = translateGLM * scaleGLM * rotateGLM;
	Mat4x4f translateScaleRML, modelRML;
	mat4x4f_mul_mat4x4f(translateRML, scaleRML, translateScaleRML);
	mat4x4f_mul_mat4x4f(translateScaleRML, rotateRML, modelRML);
	compare_matrices(modelRML, modelGLM);

	// 6
	float cameraX = 0.0;
	float cameraY = 0.0;
	float cameraZ = 1.0;
	glm::mat4 viewGLM = glm::lookAt(glm::vec3(cameraX, cameraY, cameraZ), glm::vec3(0.0), glm::vec3(0.0, 1.0, 0.0));
	Mat4x4f viewRML;
	mat4x4f_mul_mat4x4f((Mat4x4f) {
		1.0, 0.0, 0.0, 0.0, /* Right vector */
		0.0, 1.0, 0.0, 0.0, /* Up vector */
		0.0, 0.0, 1.0, 0.0, /* Direction vector */
		0.0, 0.0, 0.0, 1.0
	}, (Mat4x4f) {
		1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		-cameraX, -cameraY, -cameraZ, 1.0
	}, viewRML);
	compare_matrices(viewRML, viewGLM);

	// 7
	float fov = M_PI * 0.5;
	float aspect = 1.0;
	float near = 0.1;
	float far = 100.0;
	glm::mat4 projectionGLM = glm::perspective(fov, aspect, near, far);
	Mat4x4f projectionRML = {
		1.0 / (aspect * tan(fov * 0.5)), 0.0, 0.0, 0.0,
		0.0, 1.0 / tan(fov * 0.5), 0.0, 0.0,
		0.0, 0.0, -(far + near) / (far - near), -1.0,
		0.0, 0.0, -2.0 * far * near / (far - near), 0.0
	};
	compare_matrices(projectionRML, projectionGLM);

	// 8
	glm::mat4 MVPGLM = projectionGLM * viewGLM * modelGLM;
	Mat4x4f projectionViewRML, MVPRML;
	mat4x4f_mul_mat4x4f(projectionRML, viewRML, projectionViewRML);
	mat4x4f_mul_mat4x4f(projectionViewRML, modelRML, MVPRML);
	compare_matrices(MVPRML, MVPGLM);

	// 9
	Vec4f vertexRML = {0.7, 0.1, -1.0, 1.0};
	Vec4f newVertexRML;
	mat4x4f_mul_vec4f(MVPRML, vertexRML, newVertexRML);
	glm::vec4 vertexGLM(0.7, 0.1, -1.0, 1.0);
	glm::vec4 newVertexGLM = MVPGLM * vertexGLM;
	compare_vectors(newVertexRML, newVertexGLM);

	const float angle = M_PI * 0.5;
	Mat4x4f rotationYRML = {
		cos(angle),		0.0,	sin(angle),	0.0,
		0.0,			1.0,	0.0,		0.0,
		-sin(angle),	0.0, 	cos(angle),	0.0,
		0.0, 			0.0, 	0.0,		1.0
};
	glm::mat4 rotationYGLM = glm::rotate(glm::mat4(1.0), rotateZ, glm::vec3(0.0, -1.0, 0.0));;
	compare_matrices(rotationYRML, rotationYGLM);

	printf("%d out of %d test units succeeded.\n", currentTestUnit - failedTestUnits, currentTestUnit);
	return 0;
}
