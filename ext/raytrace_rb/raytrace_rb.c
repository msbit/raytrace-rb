#include <ruby.h>

#include <float.h>
#include <math.h>

VALUE moduleChunkyPng = Qnil;
VALUE moduleColor = Qnil;
VALUE moduleRaytraceRb = Qnil;
VALUE classPoint = Qnil;
VALUE classRayTracer = Qnil;

void Init_raytrace_rb();

VALUE methCrossProduct(VALUE, VALUE);
VALUE methDotProduct(VALUE, VALUE);
VALUE methMinus(VALUE, VALUE);
VALUE methNormalise(VALUE);
VALUE methRender(VALUE, VALUE, VALUE, VALUE, VALUE, VALUE, VALUE, VALUE);

double methRayIntersectTriangle(VALUE, VALUE, VALUE);

void Init_raytrace_rb() {
  moduleChunkyPng = rb_define_module("ChunkyPNG");
  moduleRaytraceRb = rb_define_module("RaytraceRb");

  classPoint = rb_define_class_under(moduleRaytraceRb, "Point", rb_cObject);
  rb_define_method(classPoint, "cross_product", methCrossProduct, 1);
  rb_define_method(classPoint, "dot_product", methDotProduct, 1);
  rb_define_method(classPoint, "minus", methMinus, 1);
  rb_define_method(classPoint, "normalise!", methNormalise, 0);

  classRayTracer = rb_define_class_under(moduleRaytraceRb, "RayTracer", rb_cObject);
  rb_define_method(classRayTracer, "render", methRender, 7);

  moduleColor = rb_define_module_under(moduleChunkyPng, "Color");
}

VALUE initPoint(double x, double y, double z) {
  VALUE args[3] = {rb_float_new(x), rb_float_new(y), rb_float_new(z)};

  VALUE point = rb_obj_alloc(classPoint);
  rb_obj_call_init(point, 3, args);

  return point;
}

VALUE methCrossProduct(VALUE vSelf, VALUE vOther) {
  double self_x = rb_float_value(rb_ivar_get(vSelf, rb_intern("@x")));
  double self_y = rb_float_value(rb_ivar_get(vSelf, rb_intern("@y")));
  double self_z = rb_float_value(rb_ivar_get(vSelf, rb_intern("@z")));

  double other_x = rb_float_value(rb_ivar_get(vOther, rb_intern("@x")));
  double other_y = rb_float_value(rb_ivar_get(vOther, rb_intern("@y")));
  double other_z = rb_float_value(rb_ivar_get(vOther, rb_intern("@z")));

  double x = self_y * other_z - self_z * other_y;
  double y = self_z * other_x - self_x * other_z;
  double z = self_x * other_y - self_y * other_x;

  return initPoint(x, y, z);
}

VALUE methDotProduct(VALUE vSelf, VALUE vOther) {
  double self_x = rb_float_value(rb_ivar_get(vSelf, rb_intern("@x")));
  double self_y = rb_float_value(rb_ivar_get(vSelf, rb_intern("@y")));
  double self_z = rb_float_value(rb_ivar_get(vSelf, rb_intern("@z")));

  double other_x = rb_float_value(rb_ivar_get(vOther, rb_intern("@x")));
  double other_y = rb_float_value(rb_ivar_get(vOther, rb_intern("@y")));
  double other_z = rb_float_value(rb_ivar_get(vOther, rb_intern("@z")));

  double dot_product = self_x * other_x + self_y * other_y + self_z * other_z;

  return rb_float_new(dot_product);
}

VALUE methMinus(VALUE vSelf, VALUE vOther) {
  double self_x = rb_float_value(rb_ivar_get(vSelf, rb_intern("@x")));
  double self_y = rb_float_value(rb_ivar_get(vSelf, rb_intern("@y")));
  double self_z = rb_float_value(rb_ivar_get(vSelf, rb_intern("@z")));

  double other_x = rb_float_value(rb_ivar_get(vOther, rb_intern("@x")));
  double other_y = rb_float_value(rb_ivar_get(vOther, rb_intern("@y")));
  double other_z = rb_float_value(rb_ivar_get(vOther, rb_intern("@z")));

  double x = self_x - other_x;
  double y = self_y - other_y;
  double z = self_z - other_z;

  return initPoint(x, y, z);
}

VALUE methNormalise(VALUE vSelf) {
  double x = rb_float_value(rb_ivar_get(vSelf, rb_intern("@x")));
  double y = rb_float_value(rb_ivar_get(vSelf, rb_intern("@y")));
  double z = rb_float_value(rb_ivar_get(vSelf, rb_intern("@z")));

  double length = sqrt(x * x + y * y + z * z);

  rb_ivar_set(vSelf, rb_intern("@x"), rb_float_new(x / length));
  rb_ivar_set(vSelf, rb_intern("@y"), rb_float_new(y / length));
  rb_ivar_set(vSelf, rb_intern("@z"), rb_float_new(z / length));

  return vSelf;
}

double methRayIntersectTriangle(VALUE vOrigin, VALUE vDirection, VALUE vTriangle) {
  VALUE edge1 = rb_ivar_get(vTriangle, rb_intern("@edge1"));
  VALUE edge2 = rb_ivar_get(vTriangle, rb_intern("@edge2"));
  VALUE vertex0 = rb_ivar_get(vTriangle, rb_intern("@vertex0"));
  double epsilon = rb_float_value(rb_const_get(rb_cFloat, rb_intern("EPSILON")));

  VALUE pvec = methCrossProduct(vDirection, edge2);
  double det = rb_float_value(methDotProduct(edge1, pvec));
  if (det < epsilon) {
    return DBL_MAX;
  }

  VALUE tvec = methMinus(vOrigin, vertex0);
  double u = rb_float_value(methDotProduct(tvec, pvec));
  if (u < 0.0 || u > det) {
    return DBL_MAX;
  }

  VALUE qvec = methCrossProduct(tvec, edge1);
  double v = rb_float_value(methDotProduct(vDirection, qvec));
  if (v < 0.0 || (u + v) > det) {
    return DBL_MAX;
  }

  double inv_det = 1.0 / det;
  double t = inv_det * rb_float_value(methDotProduct(edge2, qvec));
  if (t < epsilon) {
    return DBL_MAX;
  }

  return t;
}

VALUE methRender(VALUE vSelf, VALUE vWidth, VALUE vHeight, VALUE vHorizontalFov, VALUE vVerticalFov, VALUE vTriangles, VALUE vImage, VALUE vImageName) {
  int width = NUM2INT(vWidth);
  int height = NUM2INT(vHeight);
  double horizontalFov = rb_float_value(vHorizontalFov);
  double verticalFov = rb_float_value(vVerticalFov);
  VALUE origin = initPoint(0.0, 0.0, 0.0);

  char imageName[256];
  snprintf(imageName, 256, "%d.png", NUM2INT(vImageName));

  int saveChunk = NUM2INT(vWidth) / 32;
  for (int x = 0; x < width; x++) {
    double azimuth = (((double) x * horizontalFov) / (double) width) - (horizontalFov / 2.0);
    double baseX = tan(azimuth);
    for (int y = 0; y < height; y++) {
      double attitude = (((double) y * verticalFov) / (double) height) - (verticalFov / 2.0);
      double baseY = tan(attitude);

      VALUE ray = initPoint(baseX, baseY, 1.0);
      VALUE intersectingTriangle = Qnil;

      double intersectingDistance = DBL_MAX;
      VALUE vTrianglesSize = rb_funcall(vTriangles, rb_intern("size"), 0);
      int triangleCount = NUM2INT(vTrianglesSize);
      for (int triangleIndex = 0; triangleIndex < triangleCount; triangleIndex++) {
        VALUE triangle = rb_ary_entry(vTriangles, triangleIndex);
        double t = methRayIntersectTriangle(origin, ray, triangle);
        if (t != Qnil && t < intersectingDistance) {
          intersectingDistance = t;
          intersectingTriangle = triangle;
        }
      }
      if (intersectingTriangle != Qnil) {
        double brightness = 200.0 - intersectingDistance;
        if (brightness < 0.0) {
          brightness = 0.0;
        }
        brightness /= 200.0;
        double red = NUM2DBL(rb_ivar_get(intersectingTriangle, rb_intern("@r")));
        double green = NUM2DBL(rb_ivar_get(intersectingTriangle, rb_intern("@g")));
        double blue = NUM2DBL(rb_ivar_get(intersectingTriangle, rb_intern("@b")));
        VALUE vRed = INT2FIX((int8_t)(red * brightness));
        VALUE vGreen = INT2FIX((int8_t)(green * brightness));
        VALUE vBlue = INT2FIX((int8_t)(blue * brightness));
        VALUE colour = rb_funcall(moduleColor, rb_intern("rgba"), 4, vRed, vGreen, vBlue, INT2FIX(255));
        rb_funcall(vImage, rb_intern("[]="), 3, INT2FIX(x), INT2FIX(y), colour);
      }
    }
    if (x % saveChunk == 0) {
      rb_funcall(vImage, rb_intern("save"), 1, rb_str_new2(imageName));
    }
  }
  rb_funcall(vImage, rb_intern("save"), 1, rb_str_new2(imageName));
  return Qnil;
}
