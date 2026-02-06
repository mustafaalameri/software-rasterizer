#include "wind.h"
#include <string.h>

float calculate_triangle_winding(Vec3f triangle[3]) {
	float differenceX1X0 = triangle[1][0] - triangle[0][0];
	float differenceY1Y0 = triangle[1][1] - triangle[0][1];
	float differenceX2X1 = triangle[2][0] - triangle[1][0];
	float differenceY2Y1 = triangle[2][1] - triangle[1][1];
	/* Z component of triangle normal */
	return differenceX1X0 * differenceY2Y1 - differenceY1Y0 * differenceX2X1;
}

void reverse_triangle_winding(Vec3f triangle[3]) {
	Vec4f oldVertex2;
	memcpy(oldVertex2, triangle[2], sizeof(Vec3f)); // Save V2
	memcpy(triangle[2], triangle[1], sizeof(Vec3f)); // Copy V1 -> V2
	memcpy(triangle[1], oldVertex2, sizeof(Vec3f)); // Coppy saved V2 -> V1
}

void correct_triangle_winding(Vec3f triangle[3]) {
	if (calculate_triangle_winding(triangle) > 0.0) {
		reverse_triangle_winding(triangle);
	}
}
