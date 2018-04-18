#include <ruby.h>

#include <math.h>

VALUE moduleRaytraceRb = Qnil;
VALUE classPoint = Qnil;
VALUE classRayTracer = Qnil;

ID idX = Qnil;
ID idY = Qnil;
ID idZ = Qnil;
ID idEdge1 = Qnil;
ID idEdge2 = Qnil;
ID idVertex0 = Qnil;
ID idVertex1 = Qnil;
ID idVertex2 = Qnil;
ID idEpsilon = Qnil;

void Init_raytrace_rb();

VALUE methCrossProduct(VALUE, VALUE);
VALUE methDotProduct(VALUE, VALUE);
VALUE methMinus(VALUE, VALUE);
VALUE methNormalise(VALUE);
VALUE methRayIntersectTriangle(VALUE, VALUE, VALUE, VALUE);

void Init_raytrace_rb() {
  moduleRaytraceRb = rb_define_module("RaytraceRb");

  classPoint = rb_define_class_under(moduleRaytraceRb, "Point", rb_cObject);
  rb_define_method(classPoint, "cross_product", methCrossProduct, 1);
  rb_define_method(classPoint, "dot_product", methDotProduct, 1);
  rb_define_method(classPoint, "minus", methMinus, 1);
  rb_define_method(classPoint, "normalise!", methNormalise, 0);

  classRayTracer = rb_define_class_under(moduleRaytraceRb, "RayTracer", rb_cObject);
  rb_define_method(classRayTracer, "ray_intersect_triangle", methRayIntersectTriangle, 3);

  idX = rb_to_id(rb_str_new2("@x"));
  idY = rb_to_id(rb_str_new2("@y"));
  idZ = rb_to_id(rb_str_new2("@z"));
  idEdge1 = rb_to_id(rb_str_new2("@edge1"));
  idEdge2 = rb_to_id(rb_str_new2("@edge2"));
  idVertex0 = rb_to_id(rb_str_new2("@vertex0"));
  idVertex1 = rb_to_id(rb_str_new2("@vertex1"));
  idVertex2 = rb_to_id(rb_str_new2("@vertex2"));
  idEpsilon = rb_to_id(rb_str_new2("EPSILON"));
}

VALUE methCrossProduct(VALUE self, VALUE other) {
  double self_x = rb_float_value(rb_ivar_get(self, idX));
  double self_y = rb_float_value(rb_ivar_get(self, idY));
  double self_z = rb_float_value(rb_ivar_get(self, idZ));

  double other_x = rb_float_value(rb_ivar_get(other, idX));
  double other_y = rb_float_value(rb_ivar_get(other, idY));
  double other_z = rb_float_value(rb_ivar_get(other, idZ));

  double x = self_y * other_z - self_z * other_y;
  double y = self_z * other_x - self_x * other_z;
  double z = self_x * other_y - self_y * other_x;

  VALUE args[3] = {rb_float_new(x), rb_float_new(y), rb_float_new(z)};

  VALUE point = rb_obj_alloc(classPoint);
  rb_obj_call_init(point, 3, args);

  return point;
}

VALUE methDotProduct(VALUE self, VALUE other) {
  double self_x = rb_float_value(rb_ivar_get(self, idX));
  double self_y = rb_float_value(rb_ivar_get(self, idY));
  double self_z = rb_float_value(rb_ivar_get(self, idZ));

  double other_x = rb_float_value(rb_ivar_get(other, idX));
  double other_y = rb_float_value(rb_ivar_get(other, idY));
  double other_z = rb_float_value(rb_ivar_get(other, idZ));

  double dot_product = self_x * other_x + self_y * other_y + self_z * other_z;

  return rb_float_new(dot_product);
}

VALUE methMinus(VALUE self, VALUE other) {
  double self_x = rb_float_value(rb_ivar_get(self, idX));
  double self_y = rb_float_value(rb_ivar_get(self, idY));
  double self_z = rb_float_value(rb_ivar_get(self, idZ));

  double other_x = rb_float_value(rb_ivar_get(other, idX));
  double other_y = rb_float_value(rb_ivar_get(other, idY));
  double other_z = rb_float_value(rb_ivar_get(other, idZ));

  double x = self_x - other_x;
  double y = self_y - other_y;
  double z = self_z - other_z;

  VALUE args[3] = {rb_float_new(x), rb_float_new(y), rb_float_new(z)};

  VALUE point = rb_obj_alloc(classPoint);
  rb_obj_call_init(point, 3, args);

  return point;
}

VALUE methNormalise(VALUE self) {
  double x = rb_float_value(rb_ivar_get(self, idX));
  double y = rb_float_value(rb_ivar_get(self, idY));
  double z = rb_float_value(rb_ivar_get(self, idZ));

  double length = sqrt(x * x + y * y + z * z);

  rb_ivar_set(self, idX, rb_float_new(x / length));
  rb_ivar_set(self, idY, rb_float_new(y / length));
  rb_ivar_set(self, idZ, rb_float_new(z / length));

  return self;
}

VALUE methRayIntersectTriangle(VALUE self, VALUE origin, VALUE direction, VALUE triangle) {
  VALUE edge1 = rb_ivar_get(triangle, idEdge1);
  VALUE edge2 = rb_ivar_get(triangle, idEdge2);
  VALUE vertex0 = rb_ivar_get(triangle, idVertex0);
  double epsilon = rb_float_value(rb_const_get(rb_cFloat, idEpsilon));

  VALUE pvec = methCrossProduct(direction, edge2);
  double det = rb_float_value(methDotProduct(edge1, pvec));
  if (det < epsilon) {
    return Qnil;
  }

  VALUE tvec = methMinus(origin, vertex0);
  double u = rb_float_value(methDotProduct(tvec, pvec));
  if (u < 0.0 || u > det) {
    return Qnil;
  }

  VALUE qvec = methCrossProduct(tvec, edge1);
  double v = rb_float_value(methDotProduct(direction, qvec));
  if (v < 0.0 || (u + v) > det) {
    return Qnil;
  }

  double inv_det = 1.0 / det;
  double t = inv_det * rb_float_value(methDotProduct(edge2, qvec));
  if (t < epsilon) {
    return Qnil;
  }

  return rb_float_new(t);
}
