#include "draw.h"
#include "clip.h"

#include <float.h>
#include <assert.h>

#define X_NDC_TO_SC(x) (x + 1.0) * 0.5 * bitmap.width
#define Y_NDC_TO_SC(y) (1.0 - y) * 0.5 * bitmap.height

void swapf(float *a, float *b) {
	float c = *a;
	*a = *b;
	*b = *a;
}

float edge_function(Vec3f triangle[3]) {
	return (triangle[1][0] - triangle[0][0]) * (triangle[2][1] - triangle[0][1]) - (triangle[1][1] - triangle[0][1]) * (triangle[2][0] - triangle[0][0]);
}

void put_pixel(int x, int y, uint8_t r, uint8_t g, uint8_t b, const Bitmap_t bitmap) {
	unsigned int pixelIndex = (y * bitmap.width + x) * bitmap.channelCount;
	bitmap.pData[pixelIndex++] = r;
	bitmap.pData[pixelIndex++] = g;
	bitmap.pData[pixelIndex] = b;
}

void scanline(int x0, int x1, int y, const Bitmap_t bitmap) {
	for (; x0 <= x1; x0++) {
		put_pixel(x0, y, 255, 0, 0, bitmap);
	}
}

void fill_triangle(Vec3f triangle[3], const Bitmap_t bitmap) {
	triangle[0][0] = X_NDC_TO_SC(triangle[0][0]);
	triangle[0][1] = Y_NDC_TO_SC(triangle[0][1]);
	triangle[1][0] = X_NDC_TO_SC(triangle[1][0]);
	triangle[1][1] = Y_NDC_TO_SC(triangle[1][1]);
	triangle[2][0] = X_NDC_TO_SC(triangle[2][0]);
	triangle[2][1] = Y_NDC_TO_SC(triangle[2][1]);

#if RASTERIZATION_METHOD == SCANLINE_RASTERIZATION
	float *v0 = triangle[0];
	float *v1 = triangle[1];
	float *v2 = triangle[2];

	// Sort by y
	float* tmp;
	if (v0[1] > v1[1]) { tmp = v0; v0 = v1; v1 = tmp; }
	if (v0[1] > v2[1]) { tmp = v0; v0 = v2; v2 = tmp; }
	if (v1[1] > v2[1]) { tmp = v1; v1 = v2; v2 = tmp; }

	float dy02 = v2[1] - v0[1];
	float dy01 = v1[1] - v0[1];
	float dy12 = v2[1] - v1[1];

	if (dy02 == 0) return;

	float invSlope02 = (v2[0] - v0[0]) / dy02;
	float invSlope01 = (dy01 != 0) ? (v1[0] - v0[0]) / dy01 : 0.0f;
	float invSlope12 = (dy12 != 0) ? (v2[0] - v1[0]) / dy12 : 0.0f;

	int yStart = (int) ceilf(v0[1]);
	int yMid   = (int) ceilf(v1[1]);
	int yEnd   = (int) ceilf(v2[1]);

	float xLeft  = v0[0] + (yStart - v0[1]) * invSlope02;
	float xRight = v0[0] + (yStart - v0[1]) * invSlope01;

	float *vTemp;
	if (v0[1] > v1[1]) {vTemp = v0; v0 = v1; v1 = vTemp;}
	if (v0[1] > v2[1]) {vTemp = v0; v0 = v2; v2 = vTemp;}
	if (v1[1] > v2[1]) {vTemp = v1; v1 = v2; v2 = vTemp;}

	float dyTotal = v2[1] - v0[1];
	if (dyTotal == 0) return;

	/* Top half*/
	for (int y = yStart; y < yMid; y++) {
		float x0 = xLeft;
		float x1 = xRight;

		if (x0 > x1) {
			float t = x0;
			x0 = x1;
			x1 = t;
		}

		scanline((int)x0, (int)x1, y, bitmap);

		xLeft  += invSlope02;
		xRight += invSlope01;
	}

	/* Bottom half
	Only reset right x-walker because left is sorted to land on v1 */
	xRight = v1[0] + (yMid - v1[1]) * invSlope12;

	for (int y = yMid; y < yEnd; y++) {
		float x0 = xLeft;
		float x1 = xRight;

		if (x0 > x1) {
			float t = x0;
			x0 = x1;
			x1 = t;
		}

		scanline((int)x0, (int)x1, y, bitmap);

		xLeft  += invSlope02;
		xRight += invSlope12;
	}
#else
	/* Bounding box */
#if CLIPPING_METHOD == SCREEN_SPACE_CLIPPING
	Vec2f min_bound = {
		MAX(MIN(MIN(triangle[0][0], triangle[1][0]), triangle[2][0]), 0.0),
		MAX(MIN(MIN(triangle[0][1], triangle[1][1]), triangle[2][1]), 0.0)
	};
	Vec2f max_bound = {
		MIN(MAX(MAX(triangle[0][0], triangle[1][0]), triangle[2][0]), (float) bitmap.width),
		MIN(MAX(MAX(triangle[0][1], triangle[1][1]), triangle[2][1]), (float) bitmap.height)
	};
#elif CLIPPING_METHOD == GEOMETRIC_CLIPPING
	/* Set only to triangle bounds, no need to clip to screen again */
	Vec2f min_bound = {
		MIN(MIN(triangle[0][0], triangle[1][0]), triangle[2][0]),
		MIN(MIN(triangle[0][1], triangle[1][1]), triangle[2][1])
	};
	assert(min_bound[0] >= 0.0);
	assert(min_bound[1] >= 0.0);
	Vec2f max_bound = {
		MAX(MAX(triangle[0][0], triangle[1][0]), triangle[2][0]),
		MAX(MAX(triangle[0][1], triangle[1][1]), triangle[2][1])
	};
	assert(max_bound[0] <= (float) bitmap.width);
	assert(max_bound[1] <= (float) bitmap.height);
#endif

	Vec2f p;
	for (p[1] = min_bound[1]; p[1] < max_bound[1]; p[1] += 1.0) {
		for (p[0] = min_bound[0]; p[0] < max_bound[0]; p[0] += 1.0) {
			float doubleSignedArea0 = edge_function((Vec3f[3]) {
				VEC2_FROM_VEC_INITIALIZER(triangle[0]),
				VEC2_FROM_VEC_INITIALIZER(triangle[1]),
				VEC2_FROM_VEC_INITIALIZER(p)
			});
			float doubleSignedArea1 = edge_function((Vec3f[3]) {
				VEC2_FROM_VEC_INITIALIZER(triangle[1]),
				VEC2_FROM_VEC_INITIALIZER(triangle[2]),
				VEC2_FROM_VEC_INITIALIZER(p)
			});
			float doubleSignedArea2 = edge_function((Vec3f[3]) {
				VEC2_FROM_VEC_INITIALIZER(triangle[2]),
				VEC2_FROM_VEC_INITIALIZER(triangle[0]),
				VEC2_FROM_VEC_INITIALIZER(p)
			});

			if (doubleSignedArea0 >= 0.0 && doubleSignedArea1 >= 0.0 && doubleSignedArea2 >= 0.0) {
				float doubleSignedAreaTotal = doubleSignedArea0 + doubleSignedArea1 + doubleSignedArea2;
				float weight0 = doubleSignedArea0 / doubleSignedAreaTotal;
				float weight1 = doubleSignedArea1 / doubleSignedAreaTotal;
				float weight2 = doubleSignedArea2 / doubleSignedAreaTotal;
				put_pixel((int) p[0], (int) p[1], weight0 * UINT8_MAX, weight1 * UINT8_MAX, weight2 * UINT8_MAX, bitmap);
			}
		}
	}
#endif
}
