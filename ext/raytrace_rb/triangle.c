#include <ruby.h>

#include "point.h"
#include "triangle.h"

VALUE rb_cTriangle;

struct Triangle triangleFromRb_cTriangle(const VALUE rb_iTriangle) {
  struct Triangle triangle;
  triangle.r = NUM2DBL(rb_ivar_get(rb_iTriangle, rb_intern("@r")));
  triangle.g = NUM2DBL(rb_ivar_get(rb_iTriangle, rb_intern("@g")));
  triangle.b = NUM2DBL(rb_ivar_get(rb_iTriangle, rb_intern("@b")));
  triangle.edge1 = pointFromRb_cPoint(rb_ivar_get(rb_iTriangle, rb_intern("@edge1")));
  triangle.edge2 = pointFromRb_cPoint(rb_ivar_get(rb_iTriangle, rb_intern("@edge2")));
  triangle.vertex0 = pointFromRb_cPoint(rb_ivar_get(rb_iTriangle, rb_intern("@vertex0")));
  triangle.vertex1 = pointFromRb_cPoint(rb_ivar_get(rb_iTriangle, rb_intern("@vertex1")));
  triangle.vertex2 = pointFromRb_cPoint(rb_ivar_get(rb_iTriangle, rb_intern("@vertex2")));
  return triangle;
}

void initRb_cTriangle(const VALUE under) {
  rb_cTriangle = rb_define_class_under(under, "Triangle", rb_cObject);
}

VALUE rb_cTriangleFromTriangle(const struct Triangle triangle) {
  VALUE args[6] = {
    rb_cPointFromPoint(triangle.vertex0),
    rb_cPointFromPoint(triangle.vertex1),
    rb_cPointFromPoint(triangle.vertex2),
    rb_float_new(triangle.r),
    rb_float_new(triangle.g),
    rb_float_new(triangle.b)
  };

  VALUE rb_iTriangle = rb_obj_alloc(rb_cTriangle);
  rb_obj_call_init(rb_iTriangle, 6, args);

  return rb_iTriangle;
}

struct Triangle *trianglesFromRb_cTriangles(const VALUE rb_iTriangles, int *trianglesSize) {
  *trianglesSize = NUM2INT(rb_funcall(rb_iTriangles, rb_intern("size"), 0));
  struct Triangle *triangles = malloc(sizeof(struct Triangle) * *trianglesSize);
  for (int i = 0; i < *trianglesSize; i++) {
    triangles[i] = triangleFromRb_cTriangle(rb_ary_entry(rb_iTriangles, i));
  }
  return triangles;
}
