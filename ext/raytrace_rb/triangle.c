#include <ruby.h>

#include "point.h"
#include "triangle.h"

struct Triangle triangleFromValue(VALUE vTriangle) {
  struct Triangle triangle;
  triangle.r = NUM2DBL(rb_ivar_get(vTriangle, rb_intern("@r")));
  triangle.g = NUM2DBL(rb_ivar_get(vTriangle, rb_intern("@g")));
  triangle.b = NUM2DBL(rb_ivar_get(vTriangle, rb_intern("@b")));
  triangle.edge1 = pointFromValue(rb_ivar_get(vTriangle, rb_intern("@edge1")));
  triangle.edge2 = pointFromValue(rb_ivar_get(vTriangle, rb_intern("@edge2")));
  triangle.vertex0 = pointFromValue(rb_ivar_get(vTriangle, rb_intern("@vertex0")));
  return triangle;
}
