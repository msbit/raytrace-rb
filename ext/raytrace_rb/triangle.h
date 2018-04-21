#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <ruby.h>

#include "point.h"

struct Triangle {
  double r;
  double g;
  double b;
  struct Point edge1;
  struct Point edge2;
  struct Point vertex0;
  struct Point vertex1;
  struct Point vertex2;
};

void initRb_cTriangle(const VALUE);

struct Triangle triangleFromRb_cTriangle(const VALUE);
struct Triangle *trianglesFromRb_cTriangles(const VALUE, int*);
VALUE rb_cTriangleFromTriangle(const struct Triangle);

#endif
