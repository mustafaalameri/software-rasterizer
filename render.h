#ifndef RENDER_H
#define RENDER_H

#include "bitmap.h"
#include "clip.h"
#include "draw.h"

void render_clipped_triangle(const Triangle triangle_clipped, Bitmap_t bitmap);

void render_triangle(const Triangle triangle, const Mat4x4f matrix, const Bitmap_t bitmap);

#endif
