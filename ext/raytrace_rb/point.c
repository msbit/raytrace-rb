#include <ruby.h>

#include "point.h"

VALUE rb_cPoint;
VALUE rb_mChunkyPng;
VALUE rb_mColor;

void initRb_cPoint(const VALUE under) {
  rb_cPoint = rb_define_class_under(under, "Point", rb_cObject);
  rb_define_method(rb_cPoint, "cross_product", methCrossProduct, 1);
  rb_define_method(rb_cPoint, "dot_product", methDotProduct, 1);
  rb_define_method(rb_cPoint, "minus", methMinus, 1);
  rb_define_method(rb_cPoint, "normalise!", methNormalise, 0);

  rb_mChunkyPng = rb_define_module("ChunkyPNG");
  rb_mColor = rb_define_module_under(rb_mChunkyPng, "Color");
}

VALUE methCrossProduct(const VALUE rb_iSelf, const VALUE rb_iOther) {
  struct Point self = pointFromRb_cPoint(rb_iSelf);
  struct Point other = pointFromRb_cPoint(rb_iOther);

  struct Point result = crossProduct(self, other);

  return rb_cPointFromPoint(result);
}

VALUE methDotProduct(const VALUE rb_iSelf, const VALUE rb_iOther) {
  struct Point self = pointFromRb_cPoint(rb_iSelf);
  struct Point other = pointFromRb_cPoint(rb_iOther);

  double result = dotProduct(self, other);

  return rb_float_new(result);
}

VALUE methMinus(const VALUE rb_iSelf, const VALUE rb_iOther) {
  struct Point self = pointFromRb_cPoint(rb_iSelf);
  struct Point other = pointFromRb_cPoint(rb_iOther);

  struct Point result = minus(self, other);

  return rb_cPointFromPoint(result);
}

VALUE methNormalise(VALUE rb_iSelf) {
  double x = rb_float_value(rb_ivar_get(rb_iSelf, rb_intern("@x")));
  double y = rb_float_value(rb_ivar_get(rb_iSelf, rb_intern("@y")));
  double z = rb_float_value(rb_ivar_get(rb_iSelf, rb_intern("@z")));

  double length = sqrt(x * x + y * y + z * z);

  rb_ivar_set(rb_iSelf, rb_intern("@x"), rb_float_new(x / length));
  rb_ivar_set(rb_iSelf, rb_intern("@y"), rb_float_new(y / length));
  rb_ivar_set(rb_iSelf, rb_intern("@z"), rb_float_new(z / length));

  return rb_iSelf;
}

struct Point pointFromRb_cPoint(const VALUE rb_iPoint) {
  struct Point point;
  point.x = rb_float_value(rb_ivar_get(rb_iPoint, rb_intern("@x")));
  point.y = rb_float_value(rb_ivar_get(rb_iPoint, rb_intern("@y")));
  point.z = rb_float_value(rb_ivar_get(rb_iPoint, rb_intern("@z")));
  return point;
}

VALUE rb_cPointFromPoint(const struct Point point) {
  VALUE args[3] = {rb_float_new(point.x), rb_float_new(point.y), rb_float_new(point.z)};

  VALUE rb_iPoint = rb_obj_alloc(rb_cPoint);
  rb_obj_call_init(rb_iPoint, 3, args);

  return rb_iPoint;
}

VALUE rb_cColorFromPoint(const struct Point point) {
  return rb_funcall(rb_mColor, rb_intern("rgba"), 4,
                    INT2FIX((uint8_t)point.x), INT2FIX((uint8_t)point.y),
                    INT2FIX((uint8_t)point.z), INT2FIX(255));
}

struct Point crossProduct(const struct Point self, const struct Point other) {
  struct Point product;

  product.x = self.y * other.z - self.z * other.y;
  product.y = self.z * other.x - self.x * other.z;
  product.z = self.x * other.y - self.y * other.x;

  return product;
}

double dotProduct(const struct Point self, const struct Point other) {
  return self.x * other.x + self.y * other.y + self.z * other.z;
}

struct Point minus(const struct Point self, const struct Point other) {
  struct Point result;

  result.x = self.x - other.x;
  result.y = self.y - other.y;
  result.z = self.z - other.z;

  return result;
}
