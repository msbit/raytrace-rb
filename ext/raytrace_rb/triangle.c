#include <ruby.h>

#include "point.h"
#include "triangle.h"

struct Triangle triangleFromValue(VALUE rb_iTriangle) {
  struct Triangle triangle;
  triangle.r = NUM2DBL(rb_ivar_get(rb_iTriangle, rb_intern("@r")));
  triangle.g = NUM2DBL(rb_ivar_get(rb_iTriangle, rb_intern("@g")));
  triangle.b = NUM2DBL(rb_ivar_get(rb_iTriangle, rb_intern("@b")));
  triangle.edge1 = pointFromValue(rb_ivar_get(rb_iTriangle, rb_intern("@edge1")));
  triangle.edge2 = pointFromValue(rb_ivar_get(rb_iTriangle, rb_intern("@edge2")));
  triangle.vertex0 = pointFromValue(rb_ivar_get(rb_iTriangle, rb_intern("@vertex0")));
  return triangle;
}
