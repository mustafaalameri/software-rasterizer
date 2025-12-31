#include "draw.h"
#include "clip.h"

#include <float.h>
#include <assert.h>

#define X_NDC_TO_SC(x) (x + 1.0) * 0.5 * bitmap.width
#define Y_NDC_TO_SC(y) (1.0 - y) * 0.5 * bitmap.height

float edge_function(Vec3f triangle[3]) {
	return (triangle[1][0] - triangle[0][0]) * (triangle[2][1] - triangle[0][1]) - (triangle[1][1] - triangle[0][1]) * (triangle[2][0] - triangle[0][0]);
}

void put_pixel(Vec2i p, Color_t color, Bitmap_t bitmap) {
	unsigned int pixelIndex = (p[1] * bitmap.width + p[0]) * bitmap.channelCount;
	bitmap.pData[pixelIndex++] = color.r;
	bitmap.pData[pixelIndex++] = color.g;
	bitmap.pData[pixelIndex] = color.b;
}

void fill_triangle(Vec3f triangle[3], const Bitmap_t bitmap) {
	triangle[0][0] = X_NDC_TO_SC(triangle[0][0]);
	triangle[0][1] = Y_NDC_TO_SC(triangle[0][1]);
	triangle[1][0] = X_NDC_TO_SC(triangle[1][0]);
	triangle[1][1] = Y_NDC_TO_SC(triangle[1][1]);
	triangle[2][0] = X_NDC_TO_SC(triangle[2][0]);
	triangle[2][1] = Y_NDC_TO_SC(triangle[2][1]);

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
#else
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
				//Color_t color = {pow(weight0, 1/2.2) * UINT8_MAX, pow(weight1, 1/2.2) * UINT8_MAX, pow(weight2, 1/2.2) * UINT8_MAX};
				Color_t color = {weight0 * UINT8_MAX, weight1 * UINT8_MAX, weight2 * UINT8_MAX};
				put_pixel((Vec2i) {(int) p[0], (int) p[1]}, color, bitmap);
			}
		}
	}
}
