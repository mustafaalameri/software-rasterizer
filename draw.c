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

void swapi(int *a, int *b) {
	int c = *a;
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
	Vec2i p;
	p[1] = y;
	if (x0 > x1) {
		swapi(&x0, &x1);
	}
	for (p[0] = x0; p[0] < x1; p[0] += 1) {
		put_pixel(p[0], p[1], 255, 255, 0, bitmap);
	}
}

void scanline_triangle(float *v0, float *v1, float *vPeak, const Bitmap_t bitmap) {
	float dy = v0[1] - vPeak[1];
	float invSlope0 = (v0[0] - vPeak[0]) / dy;
	float invSlope1 = (v1[0] - vPeak[0]) / dy;
	float x0 = vPeak[0];
	float x1 = vPeak[0];
	if (vPeak[1] < v0[1]) {
		for (float y = vPeak[1]; y < v0[1]; y += 1.0) {
			scanline(x0, x1, y, bitmap);
			x0 += invSlope0;
			x1 += invSlope1;
		}
	} else {
		for (float y = vPeak[1]; y > v0[1]; y -= 1.0) {
			scanline(x0, x1, y, bitmap);
			x0 -= invSlope0;
			x1 -= invSlope1;
		}
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
	float *v0 = triangle[0], *v1 = triangle[1], *v2 = triangle[2], *vTemp;
	if (v0[1] > v1[1]) {vTemp = v0; v0 = v1; v1 = vTemp;}
	if (v1[1] > v2[1]) {vTemp = v1; v1 = v2; v2 = vTemp;}
	if (v0[1] > v1[1]) {vTemp = v0; v0 = v1; v1 = vTemp;}
	/* Already have a zero-slope edge */
	if (v0[1] == v1[1]) {
		if (v0[1] != v2[1]) {
			scanline_triangle(v0, v1, v2, bitmap);
		}
	} else if (v1[1] == v2[1]) {
		if (v1[1] != v0[1]) {
			scanline_triangle(v1, v2, v0, bitmap);
		}
	} else {
		/* Split into two triangles with the edge inbetween having a solpe of zero. */
		Vec3f v3 = {v0[0] + (v1[1] - v0[1]) * (v2[0] - v0[0]) / (v2[1] - v0[1]), v1[1], 0.0};
		scanline_triangle(v1, v3, v0, bitmap);
		scanline_triangle(v1, v3, v2, bitmap);
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
