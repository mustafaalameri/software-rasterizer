/* Wrapper code for triangle presentation */
#include "render.h"
#include "wind.h"

void render_clipped_triangle(const Triangle triangle_clipped, Bitmap_t bitmap) {
	/* Perspective divide */
	Vec3f triangle_projected[3] = {
		{
			triangle_clipped[0][0] / triangle_clipped[0][3],
			triangle_clipped[0][1] / triangle_clipped[0][3],
			triangle_clipped[0][2] / triangle_clipped[0][3]
		},
		{
			triangle_clipped[1][0] / triangle_clipped[1][3],
			triangle_clipped[1][1] / triangle_clipped[1][3],
			triangle_clipped[1][2] / triangle_clipped[1][3]
		},
		{
			triangle_clipped[2][0] / triangle_clipped[2][3],
			triangle_clipped[2][1] / triangle_clipped[2][3],
			triangle_clipped[2][2] / triangle_clipped[2][3]
		}
	};
	correct_triangle_winding(triangle_projected);
	fill_triangle(triangle_projected, bitmap);
}

void render_triangle(const Triangle triangle, const Mat4x4f matrix, const Bitmap_t bitmap) {
	Triangle triangle_transformed;
	mat4x4f_mul_vec4f(matrix, triangle[0], triangle_transformed[0]);
	mat4x4f_mul_vec4f(matrix, triangle[1], triangle_transformed[1]);
	mat4x4f_mul_vec4f(matrix, triangle[2], triangle_transformed[2]);

#if CLIPPING_METHOD == GEOMETRIC_CLIPPING
	Triangle clippedTriangles[12];
	int clippedTriangleCount = 0;
	clip_triangle(triangle_transformed, clippedTriangles, &clippedTriangleCount, 3);

	for (int triangleIndex = 0; triangleIndex < clippedTriangleCount; ++triangleIndex) {
		render_clipped_triangle(clippedTriangles[triangleIndex], bitmap);
	}
#else
	render_clipped_triangle(triangle_transformed, bitmap);
#endif
}