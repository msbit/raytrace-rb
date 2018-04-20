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
};

struct Triangle triangleFromValue(VALUE);

#endif
