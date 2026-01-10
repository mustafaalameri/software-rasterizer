#ifndef CLIP_H
#define CLIP_H

#include <rml/rml.h>

#define GEOMETRIC_CLIPPING 0x000
#define SCREEN_SPACE_CLIPPING 0x001
#define GUARD_BAND_CLIPPING 0x002

#ifndef CLIPPING_METHOD
#define CLIPPING_METHOD GEOMETRIC_CLIPPING
#endif

#if CLIPPING_METHOD == GUARD_BAND_CLIPPING
/* Really depends on the number of bits allocated on memory for each position, leave for now. */
#define CLIPPING_METHOD SCREEN_SPACE_CLIPPING 
#endif

typedef Vec4f Triangle[3];

void clip_triangle(Triangle triangle, Triangle *pClippedTriangles, int *pClippedTriangleCount, int lastClippedAxisIndex);

#endif
