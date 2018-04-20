#include <ruby.h>

#include "point.h"

extern VALUE rb_cPoint;

struct Point pointFromValue(VALUE vPoint) {
  struct Point point;
  point.x = rb_float_value(rb_ivar_get(vPoint, rb_intern("@x")));
  point.y = rb_float_value(rb_ivar_get(vPoint, rb_intern("@y")));
  point.z = rb_float_value(rb_ivar_get(vPoint, rb_intern("@z")));
  return point;
}

VALUE valueFromPoint(struct Point point) {
  VALUE args[3] = {rb_float_new(point.x), rb_float_new(point.y), rb_float_new(point.z)};

  VALUE rb_iPoint = rb_obj_alloc(rb_cPoint);
  rb_obj_call_init(rb_iPoint, 3, args);

  return rb_iPoint;
}

struct Point crossProduct(struct Point self, struct Point other) {
  struct Point product;

  product.x = self.y * other.z - self.z * other.y;
  product.y = self.z * other.x - self.x * other.z;
  product.z = self.x * other.y - self.y * other.x;

  return product;
}

double dotProduct(struct Point self, struct Point other) {
  return self.x * other.x + self.y * other.y + self.z * other.z;
}

struct Point minus(struct Point self, struct Point other) {
  struct Point result;

  result.x = self.x - other.x;
  result.y = self.y - other.y;
  result.z = self.z - other.z;

  return result;
}
