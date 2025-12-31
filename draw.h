#ifndef DRAW_H
#define DRAW_H

#include "bitmap.h"
#include "rml/rml.h"

typedef struct {
	uint8_t r;
	uint8_t g;
	uint8_t b;
} Color_t;

typedef struct {
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;
} ColorAlpha_t;

void put_pixel(Vec2i p, Color_t color, Bitmap_t bitmap);

void fill_triangle(Vec3f triangle[3], const Bitmap_t bitmap);

#endif
