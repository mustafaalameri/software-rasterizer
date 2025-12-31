#include "clip.h"

#include <string.h>
#include <assert.h>
#include <float.h>
#include <stdlib.h>
#include <stdio.h>

float calculate_intersection_lerp_factor(int axisIndex, float axisDirection, Vec4f vertex0, Vec4f vertex1) {
	float c0 = vertex0[axisIndex];
	float c1 = vertex1[axisIndex];
	float differenceC0C1 = c0 - c1;
	/* If near plane */
	if (axisIndex == 2 && axisDirection < 0.0) {
		return c0 / differenceC0C1;
	}

	float w0 = vertex0[3];
	float w1 = vertex1[3];
	float differenceW1W0 = w1 - w0;

	return (c0 - axisDirection * w0) / (axisDirection * differenceW1W0 + differenceC0C1);
}

/* Clip a triangle, and return a 12-element-max list of clipped triangles not containing the given triangle.
Set lastClippedAxisIndex to -1 to indicate the absence of recursion. */
void clip_triangle(Triangle triangle, Triangle *pClippedTriangles, int *pClippedTriangleCount, int lastClippedAxisIndex) {
	for (float axisDirection = -1.0; axisDirection <= 1.0; axisDirection += 2.0) {
		for (int axisIndex = lastClippedAxisIndex - 1; axisIndex > -1; axisIndex--) {
			int outsideVerticesIndices[3];
			int outsideVerticesCount = 0;
			int insideVerticesIndices[3];
			int insideVerticesCount = 0;
			float planeDistance;
			for (int vertexIndex = 0; vertexIndex < 3; vertexIndex++) {
				if (axisDirection > 0.0) {
					if (triangle[vertexIndex][axisIndex] > triangle[vertexIndex][3]) {
						outsideVerticesIndices[outsideVerticesCount++] = vertexIndex;
					} else {
						insideVerticesIndices[insideVerticesCount++] = vertexIndex;
					}
				} else {
					/* If near plane, compare against zero. Otherwise, compare against W. */
					if (axisIndex == 2) {
						planeDistance = 0.0;
					} else {
						planeDistance = triangle[vertexIndex][3];
					}
					if (triangle[vertexIndex][axisIndex] < -planeDistance) {
						outsideVerticesIndices[outsideVerticesCount++] = vertexIndex;
					} else {
						insideVerticesIndices[insideVerticesCount++] = vertexIndex;
					}
				}
			}
			assert(insideVerticesCount + outsideVerticesCount == 3);

			if (insideVerticesCount == 0) {
				return;
			}
			if (insideVerticesCount == 2) {
				float lerpFactor0 = calculate_intersection_lerp_factor(
					axisIndex,
					axisDirection,
					triangle[insideVerticesIndices[0]],
					triangle[outsideVerticesIndices[0]]
				);
				assert(lerpFactor0 >= 0.0);
				assert(lerpFactor0 <= 1.0);
				float lerpFactor1 = calculate_intersection_lerp_factor(
					axisIndex,
					axisDirection,
					triangle[insideVerticesIndices[1]],
					triangle[outsideVerticesIndices[0]]
				);
				assert(lerpFactor1 >= 0.0);
				assert(lerpFactor1 <= 1.0);

				Vec4f newInsideVertex0 = {
					lerpf(triangle[insideVerticesIndices[0]][0], triangle[outsideVerticesIndices[0]][0], lerpFactor0),
					lerpf(triangle[insideVerticesIndices[0]][1], triangle[outsideVerticesIndices[0]][1], lerpFactor0),
					lerpf(triangle[insideVerticesIndices[0]][2], triangle[outsideVerticesIndices[0]][2], lerpFactor0),
					lerpf(triangle[insideVerticesIndices[0]][3], triangle[outsideVerticesIndices[0]][3], lerpFactor0)
				};
				Vec4f newInsideVertex1 = {
					lerpf(triangle[insideVerticesIndices[1]][0], triangle[outsideVerticesIndices[0]][0], lerpFactor1),
					lerpf(triangle[insideVerticesIndices[1]][1], triangle[outsideVerticesIndices[0]][1], lerpFactor1),
					lerpf(triangle[insideVerticesIndices[1]][2], triangle[outsideVerticesIndices[0]][2], lerpFactor1),
					lerpf(triangle[insideVerticesIndices[1]][3], triangle[outsideVerticesIndices[0]][3], lerpFactor1)
				};
#ifndef NDEBUG
				/* Should probably set instead of assert, for performance */
				if (axisIndex == 2 && axisDirection < 0.0) {
					/*assert(almost_equals(newInsideVertex0[2], 0.0, FLT_EPSILON));
					assert(almost_equals(newInsideVertex1[2], 0.0, FLT_EPSILON));
					assert(newInsideVertex0[2] == 0.0);
					assert(newInsideVertex1[2] == 0.0);*/
				} else {
					/*assert(almost_equals(newInsideVertex0[axisIndex], axisDirection, FLT_EPSILON));
					assert(almost_equals(newInsideVertex1[axisIndex], axisDirection, FLT_EPSILON));
					assert(newInsideVertex0[axisIndex] == axisDirection);
					assert(newInsideVertex1[axisIndex] == axisDirection);*/
				}
#endif

				/* We get a quad made of two triangles */
				Triangle newTriangle0 = {
					VEC4_FROM_VEC_INITIALIZER(triangle[insideVerticesIndices[0]]),
					VEC4_FROM_VEC_INITIALIZER(newInsideVertex0),
					VEC4_FROM_VEC_INITIALIZER(triangle[insideVerticesIndices[1]])
				};
				Triangle newTriangle1 = {
					VEC4_FROM_VEC_INITIALIZER(newInsideVertex0),
					VEC4_FROM_VEC_INITIALIZER(newInsideVertex1),
					VEC4_FROM_VEC_INITIALIZER(triangle[insideVerticesIndices[1]])
				};

				if (lastClippedAxisIndex > 0) {
					clip_triangle(newTriangle0, pClippedTriangles, pClippedTriangleCount, axisIndex);
					clip_triangle(newTriangle1, pClippedTriangles, pClippedTriangleCount, axisIndex);
				}

				return;
			}
			if (insideVerticesCount == 1) {
				float lerpFactor0 = calculate_intersection_lerp_factor(
					axisIndex,
					axisDirection,
					triangle[insideVerticesIndices[0]],
					triangle[outsideVerticesIndices[0]]
				);
				float lerpFactor1 = calculate_intersection_lerp_factor(
					axisIndex,
					axisDirection,
					triangle[insideVerticesIndices[0]],
					triangle[outsideVerticesIndices[1]]
				);

				triangle[outsideVerticesIndices[0]][0] = lerpf(triangle[insideVerticesIndices[0]][0], triangle[outsideVerticesIndices[0]][0], lerpFactor0);
				triangle[outsideVerticesIndices[0]][1] = lerpf(triangle[insideVerticesIndices[0]][1], triangle[outsideVerticesIndices[0]][1], lerpFactor0);
				triangle[outsideVerticesIndices[0]][2] = lerpf(triangle[insideVerticesIndices[0]][2], triangle[outsideVerticesIndices[0]][2], lerpFactor0);
				triangle[outsideVerticesIndices[0]][3] = lerpf(triangle[insideVerticesIndices[0]][3], triangle[outsideVerticesIndices[0]][3], lerpFactor0);

				triangle[outsideVerticesIndices[1]][0] = lerpf(triangle[insideVerticesIndices[0]][0], triangle[outsideVerticesIndices[1]][0], lerpFactor1);
				triangle[outsideVerticesIndices[1]][1] = lerpf(triangle[insideVerticesIndices[0]][1], triangle[outsideVerticesIndices[1]][1], lerpFactor1);
				triangle[outsideVerticesIndices[1]][2] = lerpf(triangle[insideVerticesIndices[0]][2], triangle[outsideVerticesIndices[1]][2], lerpFactor1);
				triangle[outsideVerticesIndices[1]][3] = lerpf(triangle[insideVerticesIndices[0]][3], triangle[outsideVerticesIndices[1]][3], lerpFactor1);
			}
		}
	}
	memcpy(&(pClippedTriangles[(*pClippedTriangleCount)++]), triangle, sizeof(Triangle));
}
