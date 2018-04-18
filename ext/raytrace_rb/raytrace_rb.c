#include <ruby.h>

#include <float.h>
#include <math.h>
#include <pthread.h>

struct Point {
  double x;
  double y;
  double z;
};

struct Triangle {
  double r;
  double g;
  double b;
  struct Point edge1;
  struct Point edge2;
  struct Point vertex0;
};

struct ThreadArgument {
  int trianglesSize;
  struct Point origin;
  struct Point ray;
  struct Triangle *triangles;
};

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

struct Point crossProduct(struct Point, struct Point);

double rayIntersectTriangle(struct Point, struct Point, struct Triangle);

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

struct Point pointFromValue(VALUE vPoint) {
  struct Point point;
  point.x = rb_float_value(rb_ivar_get(vPoint, rb_intern("@x")));
  point.y = rb_float_value(rb_ivar_get(vPoint, rb_intern("@y")));
  point.z = rb_float_value(rb_ivar_get(vPoint, rb_intern("@z")));
  return point;
}

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

struct Point crossProduct(struct Point self, struct Point other) {
  struct Point product;

  product.x = self.y * other.z - self.z * other.y;
  product.y = self.z * other.x - self.x * other.z;
  product.z = self.x * other.y - self.y * other.x;

  return product;
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

double dotProduct(struct Point self, struct Point other) {
  return self.x * other.x + self.y * other.y + self.z * other.z;
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

struct Point minus(struct Point self, struct Point other) {
  struct Point result;

  result.x = self.x - other.x;
  result.y = self.y - other.y;
  result.z = self.z - other.z;

  return result;
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

VALUE methRender(VALUE vSelf, VALUE vWidth, VALUE vHeight, VALUE vHorizontalFov, VALUE vVerticalFov, VALUE vTriangles, VALUE vImage, VALUE vImageName) {
  int width = NUM2INT(vWidth);
  int height = NUM2INT(vHeight);
  int trianglesSize = NUM2INT(rb_funcall(vTriangles, rb_intern("size"), 0));
  double horizontalFov = rb_float_value(vHorizontalFov);
  double verticalFov = rb_float_value(vVerticalFov);
  struct Point origin = {0.0, 0.0, 0.0};
  struct Triangle triangles[trianglesSize];

  for (int i = 0; i < trianglesSize; i++) {
    triangles[i] = triangleFromValue(rb_ary_entry(vTriangles, i));
  }

  char imageName[256];
  snprintf(imageName, 256, "%d.png", NUM2INT(vImageName));

  int saveChunk = NUM2INT(vWidth) / 32;
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
      rb_funcall(vImage, rb_intern("[]="), 3, INT2FIX(x), INT2FIX(y), INT2NUM(*colour));
      free(colour);
    }
    if (x % saveChunk == 0) {
      rb_funcall(vImage, rb_intern("save"), 1, rb_str_new2(imageName));
    }
  }
  rb_funcall(vImage, rb_intern("save"), 1, rb_str_new2(imageName));
  return Qnil;
}
