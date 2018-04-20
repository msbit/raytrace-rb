#include <ruby.h>

#include <float.h>
#include <math.h>
#include <pthread.h>

#include "point.h"
#include "triangle.h"

#define MAX_DEPTH 5

struct ThreadArgument {
  int trianglesSize;
  struct Point origin;
  struct Point direction;
  struct Triangle *triangles;
};

VALUE rb_mChunkyPng = Qnil;
VALUE rb_mColor = Qnil;
VALUE rb_mRaytraceRb = Qnil;

extern VALUE rb_cPoint;
VALUE rb_cRayTracer = Qnil;
extern VALUE rb_cTriangle;

void Init_raytrace_rb();

VALUE methRender(VALUE, VALUE, VALUE, VALUE, VALUE, VALUE, VALUE, VALUE);
double intersectsTriangle(struct Point, struct Point, struct Triangle);
struct Point *castRay(struct Point, struct Point, struct Triangle*, int, int);

void Init_raytrace_rb() {
  rb_mChunkyPng = rb_define_module("ChunkyPNG");
  rb_mRaytraceRb = rb_define_module("RaytraceRb");

  initRb_cPoint(rb_mRaytraceRb);
  initRb_cTriangle(rb_mRaytraceRb);

  rb_cRayTracer = rb_define_class_under(rb_mRaytraceRb, "RayTracer", rb_cObject);
  rb_define_method(rb_cRayTracer, "render", methRender, 7);

  rb_mColor = rb_define_module_under(rb_mChunkyPng, "Color");
}

void *performWork(void *argument) {
  struct ThreadArgument value = *((struct ThreadArgument *) argument);
  struct Point *colour;

  colour = castRay(value.origin, value.direction, value.triangles, value.trianglesSize, 0);
  pthread_exit(colour);
}

struct Point *castRay(struct Point origin, struct Point direction, struct Triangle *triangles, int trianglesSize, int depth) {
  struct Point *hitColour = malloc(sizeof(struct Point));
  hitColour->x = 0.0;
  hitColour->y = 0.0;
  hitColour->z = 0.0;

  if (depth > MAX_DEPTH) {
    return hitColour;
  }

  double nearestDistance = DBL_MAX;
  struct Triangle nearestTriangle;
  for (int triangleIndex = 0; triangleIndex < trianglesSize; triangleIndex++) {
    struct Triangle triangle = triangles[triangleIndex];
    double t = intersectsTriangle(origin, direction, triangle);
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
    hitColour->x = nearestTriangle.r * brightness;
    hitColour->y = nearestTriangle.g * brightness;
    hitColour->z = nearestTriangle.b * brightness;
  }
  return hitColour;
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
    triangles[i] = triangleFromRb_cTriangle(rb_ary_entry(rb_iTriangles, i));
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
      threadArgs[y].direction = (struct Point){baseX, baseY, 1.0};
      threadArgs[y].triangles = triangles;
      threadArgs[y].trianglesSize = trianglesSize;
      pthread_create(&threads[y], NULL, performWork, &threadArgs[y]);
    }
    for (int y = 0; y < height; y++) {
      struct Point *colour;
      pthread_join(threads[y], (void**)&colour);
      rb_funcall(rb_iImage, rb_intern("[]="), 3, INT2FIX(x), INT2FIX(y), rb_cColorFromPoint(*colour));
      free(colour);
    }
    if (x % saveChunk == 0) {
      rb_funcall(rb_iImage, rb_intern("save"), 1, rb_str_new2(imageName));
    }
  }
  rb_funcall(rb_iImage, rb_intern("save"), 1, rb_str_new2(imageName));
  return Qnil;
}

double intersectsTriangle(struct Point origin, struct Point direction, struct Triangle triangle) {
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
