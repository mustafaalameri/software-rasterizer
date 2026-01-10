#ifndef DRAW_H
#define DRAW_H

#define SCANLINE_RASTERIZATION 0x003
#define PINEDA_RASTERIZATION 0x004 /* https://dl.acm.org/doi/10.1145/54852.378457 */

#ifndef RASTERIZATION_METHOD
#define RASTERIZATION_METHOD SCANLINE_RASTERIZATION
#endif

#include "bitmap.h"
#include "rml/rml.h"

/* Still going to occupy an additional byte of padding */
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

void put_pixel(int x, int y, uint8_t r, uint8_t g, uint8_t b, const Bitmap_t bitmap);

void fill_triangle(Vec3f triangle[3], const Bitmap_t bitmap);

#endif
