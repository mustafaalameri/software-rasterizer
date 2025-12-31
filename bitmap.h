#ifndef BITMAP_H
#define BITMAP_H

#include <stdint.h>

typedef struct {
	uint8_t *pData;
	int32_t width;
	int32_t height;
	uint8_t channelCount;
	int32_t stride;
} Bitmap_t;

int calculate_stride(int width, int channelCount);

Bitmap_t create_bitmap(const int width, const int height, const int channelCount);

void blit_bitmap(Bitmap_t source, int sourceOffsetX, int sourceOffsetY, Bitmap_t destination, int destinationOffsetX, int destinationOffsetY, int width, int height);

void clear_bitmap(Bitmap_t bitmap);

#endif
