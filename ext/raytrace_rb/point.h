#ifndef POINT_H
#define POINT_H

#include <ruby.h>

struct Point {
  double x;
  double y;
  double z;
};

void initRb_cPoint(VALUE);

VALUE methCrossProduct(VALUE, VALUE);
VALUE methDotProduct(VALUE, VALUE);
VALUE methMinus(VALUE, VALUE);
VALUE methNormalise(VALUE);

double dotProduct(struct Point, struct Point);
struct Point crossProduct(struct Point, struct Point);
struct Point minus(struct Point, struct Point);
struct Point pointFromRb_cPoint(VALUE);
VALUE rb_cColorFromPoint(struct Point);
VALUE rb_cPointFromPoint(struct Point);

#endif
