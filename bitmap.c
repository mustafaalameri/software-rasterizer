#include "bitmap.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

typedef struct {
	int16_t signature;
	int32_t fileSize;
	int16_t reserved0;
	int16_t reserved1;
	int32_t bitmapDataOffset;
} __attribute__((packed, aligned(1))) BMPHeader_t;

typedef struct {
	int32_t infoHeaderSize;
	int32_t bitmapWidth;
	int32_t bitmapHeight;
	int16_t planeCount;
	int16_t bitsPerPixel;
	int32_t compressionMethod;
	int32_t compressedBitmapSize;
	int32_t resolutionX;
	int32_t resolutionY;
	int32_t colorsUsed;
	int32_t importantColors;
} __attribute__((packed, aligned(1))) BMPInfoHeader_t;

int calculate_stride(int width, int channelCount) {
	int bitsPerPixel = channelCount * 8;
	int scanlineBytes = width * (bitsPerPixel / 8);
	return (scanlineBytes + 3) & ~3;
}

Bitmap_t create_bitmap(const int width, const int height, const int channelCount) {
	Bitmap_t bitmap = {0};
	bitmap.width = width;
	bitmap.height = height;
	bitmap.channelCount = channelCount;
	bitmap.stride = calculate_stride(width, channelCount);
	bitmap.pData = calloc(bitmap.stride * height * channelCount, sizeof(uint8_t));
	if (bitmap.pData == NULL) {
		perror("calloc");
		exit(EXIT_FAILURE);
	}

	return bitmap;
}

void blit_bitmap(Bitmap_t source, int sourceOffsetX, int sourceOffsetY, Bitmap_t destination, int destinationOffsetX, int destinationOffsetY, int width, int height) {
    assert(source.channelCount == destination.channelCount);

    for (unsigned int i = 0; i < height; ++i) {
        memcpy(
			destination.pData + (destinationOffsetY + i) * destination.stride + destinationOffsetX * destination.channelCount, 
            source.pData + (sourceOffsetY + i) * source.stride + sourceOffsetX * source.channelCount,
            width * source.channelCount
		);
    }
}

void clear_bitmap(Bitmap_t bitmap) {
	memset(bitmap.pData, UINT8_MAX, bitmap.stride * bitmap.height * sizeof(uint8_t));
}

/*void write_bitmap_to_bmp(const Bitmap_t bitmap, const char *strFilename) {
	FILE *pFile = fopen(strFilename, "wb");
	if (pFile == NULL) {
		perror("fopen");
		exit(EXIT_FAILURE);
	}

	int32_t bitmapDataOffset = sizeof(BMPHeader_t) + sizeof(BMPInfoHeader_t);
	BMPHeader_t header = {
		0x4d42,
		bitmapDataOffset + bitmap.subPixelCount,
		0x0,
		0x0,
		bitmapDataOffset,
	};
	fwrite(&header, sizeof(header), 1, pFile);

	BMPInfoHeader_t infoHeader = {
		sizeof(BMPInfoHeader_t),
		bitmap.width,
		-bitmap.height, /* Top-to-bottom bitmap, negative height * /
		1,
		24,
		0,
		0,
		0,
		0,
		256,
		0
	};
	fwrite(&infoHeader, sizeof(infoHeader), 1, pFile);

	for (unsigned int i = 0; i < bitmap.width * bitmap.height * bitmap.channelCount; i += 3) { /* BMPs are BGRA, not RGBA * /
		uint8_t red = bitmap.pData[i];
		bitmap.pData[i] = bitmap.pData[i + 2];
		bitmap.pData[i + 2] = red;
	}
	fwrite(bitmap.pData, bitmap.width * bitmap.height * bitmap.channelCount * sizeof(uint8_t), 1, pFile);

	fclose(pFile);
}*/
