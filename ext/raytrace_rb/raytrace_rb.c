#include <ruby.h>

#include <float.h>
#include <math.h>
#include <pthread.h>

#include "point.h"
#include "triangle.h"

struct ThreadArgument {
  int trianglesSize;
  struct Point origin;
  struct Point ray;
  struct Triangle *triangles;
};

VALUE rb_mChunkyPng = Qnil;
VALUE rb_mColor = Qnil;
VALUE rb_mRaytraceRb = Qnil;
VALUE rb_cPoint = Qnil;
VALUE rb_cRayTracer = Qnil;
VALUE rb_cTriangle = Qnil;

void Init_raytrace_rb();

VALUE methCrossProduct(VALUE, VALUE);
VALUE methDotProduct(VALUE, VALUE);
VALUE methMinus(VALUE, VALUE);
VALUE methNormalise(VALUE);
VALUE methRender(VALUE, VALUE, VALUE, VALUE, VALUE, VALUE, VALUE, VALUE);

double rayIntersectTriangle(struct Point, struct Point, struct Triangle);

void Init_raytrace_rb() {
  rb_mChunkyPng = rb_define_module("ChunkyPNG");
  rb_mRaytraceRb = rb_define_module("RaytraceRb");

  rb_cPoint = rb_define_class_under(rb_mRaytraceRb, "Point", rb_cObject);
  rb_define_method(rb_cPoint, "cross_product", methCrossProduct, 1);
  rb_define_method(rb_cPoint, "dot_product", methDotProduct, 1);
  rb_define_method(rb_cPoint, "minus", methMinus, 1);
  rb_define_method(rb_cPoint, "normalise!", methNormalise, 0);

  rb_cRayTracer = rb_define_class_under(rb_mRaytraceRb, "RayTracer", rb_cObject);
  rb_define_method(rb_cRayTracer, "render", methRender, 7);

  rb_cTriangle = rb_define_class_under(rb_mRaytraceRb, "Triangle", rb_cObject);

  rb_mColor = rb_define_module_under(rb_mChunkyPng, "Color");
}

VALUE methCrossProduct(VALUE rb_iSelf, VALUE rb_iOther) {
  struct Point self = pointFromValue(rb_iSelf);
  struct Point other = pointFromValue(rb_iOther);

  struct Point result = crossProduct(self, other);

  return valueFromPoint(result);
}

VALUE methDotProduct(VALUE rb_iSelf, VALUE rb_iOther) {
  struct Point self = pointFromValue(rb_iSelf);
  struct Point other = pointFromValue(rb_iOther);

  double result = dotProduct(self, other);

  return rb_float_new(result);
}

VALUE methMinus(VALUE rb_iSelf, VALUE rb_iOther) {
  struct Point self = pointFromValue(rb_iSelf);
  struct Point other = pointFromValue(rb_iOther);

  struct Point result = minus(self, other);

  return valueFromPoint(result);
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


double rayIntersectTriangle(struct Point origin, struct Point direction, struct Triangle triangle) {
  double epsilon = rb_float_value(rb_const_get(rb_cFloat, rb_intern("EPSILON")));

  struct Point pvec = crossProduct(direction, triangle.edge2);
  double det = dotProduct(triangle.edge1, pvec);
  if (det < epsilon) {
    return DBL_MAX;
  }

  struct Point tvec = minus(origin, triangle.vertex0);
  double u = dotProduct(tvec, pvec);
  if (u < 0.0 || u > det) {
    return DBL_MAX;
  }

  struct Point qvec = crossProduct(tvec, triangle.edge1);
  double v = dotProduct(direction, qvec);
  if (v < 0.0 || (u + v) > det) {
    return DBL_MAX;
  }

  double inv_det = 1.0 / det;
  double t = inv_det * dotProduct(triangle.edge2, qvec);
  if (t < epsilon) {
    return DBL_MAX;
  }

  return t;
}

void *performWork(void *argument) {
  struct ThreadArgument value = *((struct ThreadArgument *) argument);
  struct Triangle nearestTriangle;
  int *colour = malloc(sizeof(int));
  *colour = 255;

  double nearestDistance = DBL_MAX;
  for (int triangleIndex = 0; triangleIndex < value.trianglesSize; triangleIndex++) {
    struct Triangle triangle = value.triangles[triangleIndex];
    double t = rayIntersectTriangle(value.origin, value.ray, triangle);
    if (t < nearestDistance) {
      nearestDistance = t;
      nearestTriangle = triangle;
    }
  }
  if (nearestDistance < DBL_MAX) {
    double brightness = 200.0 - nearestDistance;
    if (brightness < 0.0) {
      brightness = 0.0;
    }
    brightness /= 200.0;
    *colour = ((int8_t)(nearestTriangle.r * brightness) << 24) |
              ((int8_t)(nearestTriangle.g * brightness) << 16) |
              ((int8_t)(nearestTriangle.b * brightness) << 8) |
              255;
  }
  pthread_exit(colour);
}

VALUE methRender(VALUE rb_iSelf, VALUE rb_iWidth, VALUE rb_iHeight, VALUE rb_iHorizontalFov, VALUE rb_iVerticalFov, VALUE rb_iTriangles, VALUE rb_iImage, VALUE rb_iRandomSeed) {
  int width = NUM2INT(rb_iWidth);
  int height = NUM2INT(rb_iHeight);
  int trianglesSize = NUM2INT(rb_funcall(rb_iTriangles, rb_intern("size"), 0));
  double horizontalFov = rb_float_value(rb_iHorizontalFov);
  double verticalFov = rb_float_value(rb_iVerticalFov);
  struct Point origin = {0.0, 0.0, 0.0};
  struct Triangle triangles[trianglesSize];

  for (int i = 0; i < trianglesSize; i++) {
    triangles[i] = triangleFromValue(rb_ary_entry(rb_iTriangles, i));
  }

  VALUE rb_iRandomSeedString = rb_big2str(rb_iRandomSeed, 10);
  char *randomSeed = StringValueCStr(rb_iRandomSeedString);

  char imageName[256];
  snprintf(imageName, 256, "%s.png", randomSeed);

  int saveChunk = NUM2INT(rb_iWidth) / 32;
  for (int x = 0; x < width; x++) {
    double azimuth = (((double) x * horizontalFov) / (double) width) - (horizontalFov / 2.0);
    double baseX = tan(azimuth);

    pthread_t threads[height];
    struct ThreadArgument threadArgs[height];

    for (int y = 0; y < height; y++) {
      double attitude = (((double) y * verticalFov) / (double) height) - (verticalFov / 2.0);
      double baseY = tan(attitude);

      threadArgs[y].origin = origin;
      threadArgs[y].ray = (struct Point){baseX, baseY, 1.0};
      threadArgs[y].triangles = triangles;
      threadArgs[y].trianglesSize = trianglesSize;
      pthread_create(&threads[y], NULL, performWork, &threadArgs[y]);
    }
    for (int y = 0; y < height; y++) {
      int* colour;
      pthread_join(threads[y], (void**)&colour);
      rb_funcall(rb_iImage, rb_intern("[]="), 3, INT2FIX(x), INT2FIX(y), INT2NUM(*colour));
      free(colour);
    }
    if (x % saveChunk == 0) {
      rb_funcall(rb_iImage, rb_intern("save"), 1, rb_str_new2(imageName));
    }
  }
  rb_funcall(rb_iImage, rb_intern("save"), 1, rb_str_new2(imageName));
  return Qnil;
}
