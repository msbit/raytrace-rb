#ifndef POINT_H
#define POINT_H

#include <ruby.h>

struct Point {
  double x;
  double y;
  double z;
};

void initRb_cPoint(const VALUE);

VALUE methCrossProduct(const VALUE, const VALUE);
VALUE methDotProduct(const VALUE, const VALUE);
VALUE methMinus(const VALUE, const VALUE);
VALUE methNormalise(VALUE);

double dotProduct(const struct Point, const struct Point);
struct Point crossProduct(const struct Point, const struct Point);
struct Point minus(const struct Point, const struct Point);
struct Point pointFromRb_cPoint(const VALUE);
VALUE rb_cColorFromPoint(const struct Point);
VALUE rb_cPointFromPoint(const struct Point);

#endif
