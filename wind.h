#ifndef WIND_H
#define WIND_H

#include "clip.h"

float calculate_triangle_winding(Vec3f triangle[3]);

void reverse_triangle_winding(Vec3f triangle[3]);

void correct_triangle_winding(Vec3f triangle[3]);

#endif
