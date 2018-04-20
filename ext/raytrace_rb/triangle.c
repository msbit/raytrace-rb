#include <ruby.h>

#include "point.h"
#include "triangle.h"

extern VALUE rb_cTriangle;

struct Triangle triangleFromValue(VALUE rb_iTriangle) {
  struct Triangle triangle;
  triangle.r = NUM2DBL(rb_ivar_get(rb_iTriangle, rb_intern("@r")));
  triangle.g = NUM2DBL(rb_ivar_get(rb_iTriangle, rb_intern("@g")));
  triangle.b = NUM2DBL(rb_ivar_get(rb_iTriangle, rb_intern("@b")));
  triangle.edge1 = pointFromValue(rb_ivar_get(rb_iTriangle, rb_intern("@edge1")));
  triangle.edge2 = pointFromValue(rb_ivar_get(rb_iTriangle, rb_intern("@edge2")));
  triangle.vertex0 = pointFromValue(rb_ivar_get(rb_iTriangle, rb_intern("@vertex0")));
  triangle.vertex1 = pointFromValue(rb_ivar_get(rb_iTriangle, rb_intern("@vertex1")));
  triangle.vertex2 = pointFromValue(rb_ivar_get(rb_iTriangle, rb_intern("@vertex2")));
  return triangle;
}

VALUE valueFromTriangle(struct Triangle triangle) {
  VALUE args[6] = {
    valueFromPoint(triangle.vertex0),
    valueFromPoint(triangle.vertex1),
    valueFromPoint(triangle.vertex2),
    rb_float_new(triangle.r),
    rb_float_new(triangle.g),
    rb_float_new(triangle.b)
  };

  VALUE rb_iTriangle = rb_obj_alloc(rb_cTriangle);
  rb_obj_call_init(rb_iTriangle, 6, args);

  return rb_iTriangle;
}
