#ifndef POINT_H
#define POINT_H

#include <ruby.h>

struct Point {
  double x;
  double y;
  double z;
};

double dotProduct(struct Point, struct Point);
struct Point crossProduct(struct Point, struct Point);
struct Point minus(struct Point, struct Point);
struct Point pointFromValue(VALUE);
VALUE valueFromPoint(struct Point);

#endif
