#ifndef PIPELINE_H
#define PIPELINE_H

typedef struct currentTriangle_s {
	Vec4f positions[3];
	Vec3f colors[3];
} currentTriangle_t;
static currentTriangle_t currentTriangle;

#endif
