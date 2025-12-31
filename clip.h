#ifndef CLIP_H
#define CLIP_H

#include <rml/rml.h>

#define GEOMETRIC_CLIPPING 0x001
#define SCREEN_SPACE_CLIPPING 0x002
#define GUARD_BAND_CLIPPING 0x003

#ifndef CLIPPING_METHOD
#define CLIPPING_METHOD GEOMETRIC_CLIPPING
#endif

#if CLIPPING_METHOD == GUARD_BAND_CLIPPING
#warning Clipping method is set to guard band clipping, when rendering is done in software. Using screen space clipping as fallback.
#define CLIPPING_METHOD SCREEN_SPACE_CLIPPING 
#endif

typedef Vec4f Triangle[3];

void clip_triangle(Triangle triangle, Triangle *pClippedTriangles, int *pClippedTriangleCount, int lastClippedAxisIndex);

#endif
